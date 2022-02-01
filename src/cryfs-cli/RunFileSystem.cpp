#include "RunFileSystem.h"
#include "cryfs-cli/Cli.h"

int RunFileSystem::run() {
    int argc = 3;
    const char *argv[] = {"cryfs", "basedir", "mountdir"};
    try {
        auto &keyGenerator = cpputils::Random::OSRandom();
//        cryfs_cli::Cli(keyGenerator, cpputils::SCrypt::DefaultSettings).main(argc, argv, [] {});
        return 0;
    } catch (const std::exception &e) {
        return -1;
    }

}