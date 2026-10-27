#include <iostream>

#include "utils/config/config.h"
#include "utils/args_interpreter.h"
#include "utils/api_server.h"

#include <vector>
#include <thread>

int main(int argc, char *argv[]) {
    std::cout << "TDFS!" << std::endl;

    tdfs::Config& config = tdfs::Config::getInstance();

    tdfs::ArgsInterpreter argsInterpreter(argc, argv);
    std::vector<std::string> configPathResult = argsInterpreter.getArg("-c", 1);
    if (configPathResult.size() > 0) {
        config.configure(configPathResult.at(0));
    } else {
        config.configure();
    }
    
    std::cout << "R:   " << config.getConfigReplications() << std::endl;
    std::cout << "BS:  " << config.getConfigBlocksize() << std::endl;
    std::cout << "CND: " << config.getConfigNameDir() << std::endl;
    std::cout << "CDD: " << config.getConfigDataDir().at(0) << std::endl;
    std::cout << "IFN: " << config.getConfigIsFloatingNode() << std::endl;
    std::cout << "INN: " << config.getConfigIsNameNode() << std::endl;
    std::cout << "TSI: " << config.getConfigTcpServerInterface() << std::endl;
    std::cout << "TSP: " << config.getConfigTcpServerPort() << std::endl;
    std::cout << "CONFIG DIR: " << config.getTdfsConfigDir() << std::endl;

    tdfs::ApiServer apiServer(config.getConfigTcpServerInterface(), config.getConfigTcpServerPort());
    std::thread apiServerThread([&apiServer]() {
        apiServer.acceptClientSession();
    });

    if (apiServerThread.joinable()) {
        apiServerThread.join();
    }

    return 0;
}
