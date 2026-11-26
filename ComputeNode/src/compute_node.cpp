#include "utils/config/config.h"
#include "utils/args_interpreter.h"
#include "utils/api_server.h"
#include "utils/compute_core.h"

#include <memory>
#include <string>
#include <vector>
#include <thread>

#include <tablog.h>
#include <tablog_registry.h>

int main(int argc, char *argv[]) {
    tablog::TablogRegistry* registry = &tablog::TablogRegistry::getInstance();
    std::shared_ptr<tablog::Tablog> logger = std::make_shared<tablog::Tablog>();
    logger->configure("TDFS", true);
    registry->registerLogger("TDFS", logger);
    
    tdfs::Config& config = tdfs::Config::getInstance();

    tdfs::ArgsInterpreter argsInterpreter(argc, argv);
    std::vector<std::string> configPathResult = argsInterpreter.getArg("-c", 1);
    if (configPathResult.size() > 0) {
        config.configure(configPathResult.at(0));
    } else {
        config.configure();
    }
    
    logger->log(tablog::DEBUG, "CONFIG DIR: " + config.getTdfsConfigDir().string());

    std::shared_ptr<tdfs::ComputeCore> computeCore = std::make_shared<tdfs::ComputeCore>();
    std::thread computeThread = std::thread([&computeCore]() {
        computeCore->computeCoreCycle();
    });

    std::string interface = config.getApiServerInterface();
    int port = config.getApiServerPort();
    std::thread apiServerThread([interface, port, &computeCore]() {
        tdfs::ApiServer apiServer(interface, port, computeCore);
    });

    if (computeThread.joinable()) {
        computeThread.join();
    }
    
    if (apiServerThread.joinable()) {
        apiServerThread.join();
    }

    return 0;
}
