#include "CryfsMain.h"
#include "cryfs-cli/Cli.h"
#include <cpp-utils/io/EdsConsole.h>
#include <cpp-utils/data/EdsDataFileSystem.h>
#include "Environment.h"
#include "boost/filesystem.hpp"

namespace cryfs_cli {

    void setCryfsConfig(jobject pathnameFileSystem, const std::string &configGroupPathname) {
        auto edsDataFileSystem = std::make_shared<cpputils::EdsDataFileSystem>(
                std::make_shared<PathnameFileSystemNative>(pathnameFileSystem));
        cryfs_cli::Environment::setConfigPath(
                cpputils::FsAndPath(edsDataFileSystem, boost::filesystem::path(configGroupPathname)));
    }

    FuseSession *openFuseSession(jobject pathnameFileSystem, const std::string &groupPathname, const std::string &password) {
        int argc = 3;
        const char *argv[] = {"cryfs", groupPathname.c_str(), "/mountdir"};
        try {
            auto &keyGenerator = cpputils::Random::OSRandom();
            return cryfs_cli::Cli(keyGenerator, cpputils::SCrypt::TestSettings, std::make_shared<cpputils::EdsConsole>(password))
                    .main(pathnameFileSystem, argc, argv, [] {});
        } catch (const std::exception &e) {
            return nullptr;
        }
    }
}