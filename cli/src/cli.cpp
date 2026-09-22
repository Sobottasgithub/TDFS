#include <memory>

#include <tablog_registry.h>
#include <tablog.h>

#include "utils/cli_engine.h"

int main() {
    tablog::TablogRegistry* registry = &tablog::TablogRegistry::getInstance();
    std::shared_ptr<tablog::Tablog> logger = std::make_shared<tablog::Tablog>();
    logger->configure("TDFS-CLI", true);
    registry->registerLogger("TDFS-CLI", logger);

    tdfs::cli::CliEngine cliEngine;
    
    return 0;
}
