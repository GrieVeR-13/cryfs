#pragma once
#ifndef MESSMER_CRYFS_LOCALSTATE_LOCALSTATEDIR_H_
#define MESSMER_CRYFS_LOCALSTATE_LOCALSTATEDIR_H_

#include <cpp-utils/macros.h>
#include <boost/filesystem/path.hpp>
#include "../config/CryConfig.h"

namespace cryfs {

    class LocalStateDir final {
    public:
        LocalStateDir(cpputils::FsAndPath appDir);

        cpputils::FsAndPath forFilesystemId(const CryConfig::FilesystemID &filesystemId) const;
        cpputils::FsAndPath forBasedirMetadata() const;

    private:
        cpputils::FsAndPath _appDir;

        static void _createDirIfNotExists(const cpputils::FsAndPath &path);
    };
}


#endif
