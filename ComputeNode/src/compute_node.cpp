#include "utils/config/config.h"
#include "utils/args_interpreter.h"
#include "utils/api_server.h"
#include "utils/floating_node.h"

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
    
    logger->log(tablog::DEBUG, "R:   " + std::to_string(config.getReplications()));
    logger->log(tablog::DEBUG, "BS:  " + std::to_string(config.getBlocksize()));
    logger->log(tablog::DEBUG, "CDD: " + config.getDataDir().at(0).string());
    logger->log(tablog::DEBUG, "IFN: " + std::to_string(config.getIsFloatingNode()));
    logger->log(tablog::DEBUG, "TSI: " + config.getTcpServerInterface());
    logger->log(tablog::DEBUG, "TSP: " + std::to_string(config.getTcpServerPort()));
    logger->log(tablog::DEBUG, "UDI: " + config.getUdpDiscoveryInterface());
    logger->log(tablog::DEBUG, "UDP: " + std::to_string(config.getUdpDiscoveryPort()));
    logger->log(tablog::DEBUG, "CONFIG DIR: " + config.getTdfsConfigDir().string());

    std::string interface = config.getTcpServerInterface();
    int port = config.getTcpServerPort();
    std::thread apiServerThread([interface, port]() {
        tdfs::ApiServer apiServer(interface, port);
    });

    std::thread computeThread;
    if (config.getIsFloatingNode()) {
        computeThread = std::thread([]() {
            tdfs::FloatingNode floatingNode;  
        });
    } else {
        // TODO: implement compute node
        // also uses compute Thread
    }

    if (computeThread.joinable()) {
        computeThread.join();
    }
    
    if (apiServerThread.joinable()) {
        apiServerThread.join();
    }

    return 0;
}
