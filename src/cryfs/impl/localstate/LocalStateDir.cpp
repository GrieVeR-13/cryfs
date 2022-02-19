#include "LocalStateDir.h"
#include <boost/filesystem.hpp>

namespace bf = boost::filesystem;

namespace cryfs {
    LocalStateDir::LocalStateDir(cpputils::FsAndPath appDir): _appDir(std::move(appDir)) {}

    cpputils::FsAndPath LocalStateDir::forFilesystemId(const CryConfig::FilesystemID &filesystemId) const {
      _createDirIfNotExists(_appDir);
      cpputils::FsAndPath filesystems_dir = cpputils::FsAndPath(_appDir.getDataFileSystem(), _appDir.getPath() / "filesystems");
      _createDirIfNotExists(filesystems_dir);
      cpputils::FsAndPath this_filesystem_dir = cpputils::FsAndPath(filesystems_dir.getDataFileSystem(), _appDir.getPath() / filesystemId.ToString());
      _createDirIfNotExists(this_filesystem_dir);
      return this_filesystem_dir;
    }

    cpputils::FsAndPath LocalStateDir::forBasedirMetadata() const {
      _createDirIfNotExists(_appDir);
      return  cpputils::FsAndPath(_appDir.getDataFileSystem(), _appDir.getPath() / "basedirs");
    }

    void LocalStateDir::_createDirIfNotExists(const cpputils::FsAndPath &path) {
        if (!path.getDataFileSystem()->exists(path.getPath())) { //exception is normal
            path.getDataFileSystem()->create_directories(path.getPath());
        }
    }
}
