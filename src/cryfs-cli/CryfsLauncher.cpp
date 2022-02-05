#include "CryfsLauncher.h"
#include "cryfs-cli/Cli.h"
#include <cpp-utils/io/EdsConsole.h>

namespace cryfs_cli {
    namespace eds {

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
}