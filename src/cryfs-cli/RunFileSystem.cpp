#include "RunFileSystem.h"
#include <blockstore/implementations/eds/EdsBlockStore.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cpp-utils/assert/backtrace.h>

#include <fspp/fuse/Fuse.h>
#include <fspp/impl/FilesystemImpl.h>
#include <cpp-utils/process/subprocess.h>
#include <cpp-utils/io/DontEchoStdinToStdoutRAII.h>
#include <cryfs/impl/filesystem/CryDevice.h>
#include <cryfs/impl/config/CryConfigLoader.h>
#include <cryfs/impl/config/CryPasswordBasedKeyProvider.h>
#include "program_options/Parser.h"
#include <boost/filesystem.hpp>

#include <cryfs/impl/filesystem/CryDir.h>
#include <gitversion/gitversion.h>

#include "VersionChecker.h"
#include <gitversion/VersionCompare.h>
#include <cpp-utils/io/NoninteractiveConsole.h>
#include <cryfs/impl/localstate/LocalStateDir.h>
#include <cryfs/impl/localstate/BasedirMetadata.h>
#include "Environment.h"
#include <cryfs/impl/CryfsException.h>
#include <cpp-utils/thread/debugging.h>

using blockstore::eds::EdsBlockStore;

using cpputils::make_unique_ref;
using cpputils::NoninteractiveConsole;
//using cpputils::TempFile;
using cpputils::RandomGenerator;
using cpputils::unique_ref;
using cpputils::SCrypt;
using cpputils::either;
using cpputils::SCryptSettings;
using cpputils::Console;
using cpputils::HttpClient;
using std::cout;
using std::string;
using std::endl;
using std::shared_ptr;
using std::make_shared;
using std::unique_ptr;
using std::make_unique;
using std::function;
using boost::optional;
using boost::none;
using boost::chrono::minutes;
using boost::chrono::milliseconds;
using cpputils::dynamic_pointer_move;
using gitversion::VersionCompare;

void RunFileSystem::run() {

    try {
//        LocalStateDir localStateDir(Environment::localStateDir());
        auto blockStore = make_unique_ref<EdsBlockStore>();
//        auto config = _loadOrCreateConfig(options, localStateDir);
//        printConfig(config.oldConfig, *config.configFile->config());
        unique_ptr <fspp::fuse::Fuse> fuse = nullptr;
        bool stoppedBecauseOfIntegrityViolation = false;

//        auto onIntegrityViolation = [&fuse, &stoppedBecauseOfIntegrityViolation]() {
//            if (fuse.get() != nullptr) {
////                LOG(ERR, "Integrity violation detected. Unmounting.");
//                stoppedBecauseOfIntegrityViolation = true;
//                fuse->stop();
//            } else {
//                // Usually on an integrity violation, the file system is unmounted.
//                // Here, the file system isn't initialized yet, i.e. we failed in the initial steps when
//                // setting up _device before running initFilesystem.
//                // We can't unmount a not-mounted file system, but we can make sure it doesn't get mounted.
////                throw CryfsException("Integrity violation detected. Unmounting.", ErrorCode::IntegrityViolation);
//            }
//        };
//        const bool missingBlockIsIntegrityViolation = config.configFile->config()->missingBlockIsIntegrityViolation();
//        _device = optional < unique_ref < CryDevice
//                >> (make_unique_ref<CryDevice>(std::move(config.configFile), std::move(blockStore), std::move(localStateDir),
//                                               config.myClientId, options.allowIntegrityViolations(),
//                                               missingBlockIsIntegrityViolation, std::move(onIntegrityViolation)));
//        _sanityCheckFilesystem(_device->get());

        auto initFilesystem = [&](fspp::fuse::Fuse *fs) {
//            ASSERT(_device != none, "File system not ready to be initialized. Was it already initialized before?");

            //TODO Test auto unmounting after idle timeout
            const boost::optional<double> idle_minutes = options.unmountAfterIdleMinutes();
            _idleUnmounter = _createIdleCallback(idle_minutes, [fs, idle_minutes] {
                LOG(INFO, "Unmounting because file system was idle for {} minutes", *idle_minutes);
                fs->stop();
            });
            if (_idleUnmounter != none) {
                (*_device)->onFsAction(std::bind(&CallAfterTimeout::resetTimer, _idleUnmounter->get()));
            }

            return make_shared<fspp::FilesystemImpl>(std::move(*_device));
        };

        fuse = make_unique<fspp::fuse::Fuse>(initFilesystem, std::move(onMounted), "cryfs",
                                             "cryfs@" + options.baseDir().string());

        _initLogfile(options);

        std::cout << "\nMounting filesystem. To unmount, call:\n$ cryfs-unmount " << options.mountDir() << "\n"
                  << std::endl;

        if (options.foreground()) {
            fuse->runInForeground(options.mountDir(), options.fuseOptions());
        } else {
            fuse->runInBackground(options.mountDir(), options.fuseOptions());
        }

        if (stoppedBecauseOfIntegrityViolation) {
            throw CryfsException("Integrity violation detected. Unmounting.", ErrorCode::IntegrityViolation);
        }
    } catch (const CryfsException &e) {
        throw; // CryfsException is only thrown if setup goes wrong. Throw it through so that we get the correct process exit code.
    } catch (const std::exception &e) {
        LOG(ERR, "Crashed: {}", e.what());
    } catch (...) {
        LOG(ERR, "Crashed");
    }

}

