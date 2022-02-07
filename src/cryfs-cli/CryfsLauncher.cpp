#include "CryfsLauncher.h"
#include "cryfs-cli/Cli.h"
#include <cpp-utils/io/EdsConsole.h>
#include <cpp-utils/data/EdsDataFileSystem.h>
#include "Environment.h"
#include "boost/filesystem.hpp"

namespace cryfs_cli {

    void setCryfsConfig(jobject pathnameFileSystem, const char *configGroupPathname) {
        auto edsDataFileSystem = std::make_shared<cpputils::EdsDataFileSystem>(
                std::make_shared<PathnameFileSystemNative>(pathnameFileSystem));
        cryfs_cli::Environment::setConfigPath(
                cpputils::FsAndPath(edsDataFileSystem, boost::filesystem::path(configGroupPathname)));
    }

    int openVolume(jobject pathnameFileSystem, const char *groupPathname) {
        int argc = 3;
        const char *argv[] = {"cryfs", groupPathname, "/storage/emulated/0/cryfs/mountdir"};
        try {
            auto &keyGenerator = cpputils::Random::OSRandom();
            return cryfs_cli::Cli(keyGenerator, cpputils::SCrypt::TestSettings, std::make_shared<cpputils::EdsConsole>())
                    .main(pathnameFileSystem, argc, argv, [] {}); //todoe openmp
        } catch (const std::exception &e) {
            return -1;
        }
    }
}