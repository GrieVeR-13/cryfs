#ifndef EDS3_EDSDATAFILESYSTEM_H
#define EDS3_EDSDATAFILESYSTEM_H

#include "FsAndPath.h"
#include "filesystem/PathnameFileSystemNative.h"
#include "DataFileSystem.h"
#include <memory>

namespace cpputils {

    class EdsDataFileSystem final : public DataFileSystem {

    public:
        explicit EdsDataFileSystem(std::shared_ptr<PathnameFileSystemNative> pathnameFileSystemNative)
                : pathnameFileSystemNative(pathnameFileSystemNative) {

        }

        std::shared_ptr<PathnameFileSystemNative> getPathnameFileSystemNative() const {
            return pathnameFileSystemNative;
        };

    private:
        std::shared_ptr<PathnameFileSystemNative> pathnameFileSystemNative;
    };

}

#endif
