#include "RunFileSystem.h"

void RunFileSystem::run() {
    /*
    try {
//        LocalStateDir localStateDir(Environment::localStateDir());
        auto blockStore = make_unique_ref<OnDiskBlockStore2>(options.baseDir());
//        auto config = _loadOrCreateConfig(options, localStateDir);
//        printConfig(config.oldConfig, *config.configFile->config());
        unique_ptr <fspp::fuse::Fuse> fuse = nullptr;
        bool stoppedBecauseOfIntegrityViolation = false;

        auto onIntegrityViolation = [&fuse, &stoppedBecauseOfIntegrityViolation]() {
            if (fuse.get() != nullptr) {
                LOG(ERR, "Integrity violation detected. Unmounting.");
                stoppedBecauseOfIntegrityViolation = true;
                fuse->stop();
            } else {
                // Usually on an integrity violation, the file system is unmounted.
                // Here, the file system isn't initialized yet, i.e. we failed in the initial steps when
                // setting up _device before running initFilesystem.
                // We can't unmount a not-mounted file system, but we can make sure it doesn't get mounted.
                throw CryfsException("Integrity violation detected. Unmounting.", ErrorCode::IntegrityViolation);
            }
        };
        const bool missingBlockIsIntegrityViolation = config.configFile->config()->missingBlockIsIntegrityViolation();
        _device = optional < unique_ref < CryDevice
                >> (make_unique_ref<CryDevice>(std::move(config.configFile), std::move(blockStore), std::move(localStateDir),
                                               config.myClientId, options.allowIntegrityViolations(),
                                               missingBlockIsIntegrityViolation, std::move(onIntegrityViolation)));
        _sanityCheckFilesystem(_device->get());

        auto initFilesystem = [&](fspp::fuse::Fuse *fs) {
            ASSERT(_device != none, "File system not ready to be initialized. Was it already initialized before?");

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
     */
}

