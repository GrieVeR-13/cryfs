#include <filesystem/RandomAccessIONative.h>
#include <util.h>
#include <nativehelper/ScopedLocalRef.h>
#include <Exception.h>
#include <filesystem/FsObjectNative.h>
#include "filesystem/InputStreamNativeIStream.h"
#include "filesystem/OutputStreamNativeOStream.h"
#include "EdsDataFileSystem.h"

namespace cpputils {

    bool EdsDataFileSystem::exists(const boost::filesystem::path &path) const {
        return pathnameFileSystemNative->exists(path.string());
    }

    void EdsDataFileSystem::create_directories(const boost::filesystem::path &path) const {
        pathnameFileSystemNative->newGroup(path.string(), true);
    }


    void EdsDataFileSystem::remove(const boost::filesystem::path &path) const {
        pathnameFileSystemNative->deleteObject(path.string());
    }

    std::unique_ptr<std::istream> EdsDataFileSystem::openInputStream(const boost::filesystem::path &path) const {  //std::ios::binary ?
        try {
            auto env = getEnv();
            if (!pathnameFileSystemNative->exists(path.string(), env)) {
                throw util::Exception();
            }
            auto randomAccessReader = pathnameFileSystemNative->openRandomAccessReader(path.string(), env);
            return std::make_unique<InputStreamNativeIStream>(randomAccessReader);
        }
        catch(const util::Exception &e) {
            return std::make_unique<std::basic_istream<char>>(nullptr);
        }

    }

    std::unique_ptr<std::ostream> EdsDataFileSystem::openOutputStream(const boost::filesystem::path &path) const { //std::ios::binary | std::ios::trunc ?
        try {
            auto env = getEnv();
            if (!pathnameFileSystemNative->exists(path.string(), env)) {
                pathnameFileSystemNative->newFile(path.string(), env);
            }
            auto randomAccessIoNative = pathnameFileSystemNative->openRandomAccessIO(path.string(), env);
            return std::make_unique<OutputStreamNativeOStream>(randomAccessIoNative);
        }
        catch(const util::Exception &e) {
            return std::make_unique<std::basic_ostream<char>>(nullptr);
        }
    }

    uint64_t EdsDataFileSystem::getLength(const boost::filesystem::path &path) const { //todo move to stream
        auto fsObject = pathnameFileSystemNative->getObject(path.string());
        FsObjectNative fileSystemObject(fsObject.get());
        return fileSystemObject.getSize();
    }
}
