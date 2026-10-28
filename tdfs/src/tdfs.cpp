#include "utils/config/config.h"
#include "utils/args_interpreter.h"
#include "utils/api_server.h"

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
    
    logger->log(tablog::DEBUG, "R:   " + std::to_string(config.getConfigReplications()));
    logger->log(tablog::DEBUG, "BS:  " + std::to_string(config.getConfigBlocksize()));
    logger->log(tablog::DEBUG, "CND: " + config.getConfigNameDir().string());
    logger->log(tablog::DEBUG, "CDD: " + config.getConfigDataDir().at(0).string());
    logger->log(tablog::DEBUG, "IFN: " + std::to_string(config.getConfigIsFloatingNode()));
    logger->log(tablog::DEBUG, "INN: " + std::to_string(config.getConfigIsNameNode()));
    logger->log(tablog::DEBUG, "TSI: " + config.getConfigTcpServerInterface());
    logger->log(tablog::DEBUG, "TSP: " + std::to_string(config.getConfigTcpServerPort()));
    logger->log(tablog::DEBUG, "CONFIG DIR: " + config.getTdfsConfigDir().string());

    tdfs::ApiServer apiServer(config.getConfigTcpServerInterface(), config.getConfigTcpServerPort());
    std::thread apiServerThread([&apiServer]() {
        apiServer.acceptClientSession();
    });

    if (apiServerThread.joinable()) {
        apiServerThread.join();
    }

    return 0;
}
