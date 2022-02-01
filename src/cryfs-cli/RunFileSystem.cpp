#include "RunFileSystem.h"
#include "cryfs-cli/Cli.h"
#include <cpp-utils/io/EdsConsole.h>

int RunFileSystem::run() {
    int argc = 3;
    const char *argv[] = {"cryfs", "/storage/emulated/0/cryfs/basedir", "/storage/emulated/0/cryfs/mountdir"};
    try {
        auto &keyGenerator = cpputils::Random::OSRandom();
        cryfs_cli::Cli(keyGenerator, cpputils::SCrypt::TestSettings, std::make_shared<cpputils::EdsConsole>()).main(argc, argv, [] {});
        return 0;
    } catch (const std::exception &e) {
        return -1;
    }

}