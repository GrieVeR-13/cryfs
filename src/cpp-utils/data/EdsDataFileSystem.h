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

        bool exists(const boost::filesystem::path &path) const;

        void create_directories(const boost::filesystem::path &path) const;

        void remove(const boost::filesystem::path &path) const;

        std::shared_ptr<PathnameFileSystemNative> getPathnameFileSystemNative() const {
            return pathnameFileSystemNative;
        };

        std::unique_ptr<std::istream> openInputStream(const boost::filesystem::path &path) const;

        std::unique_ptr<std::ostream> openOutputStream(const boost::filesystem::path &path) const;

    private:
        std::shared_ptr<PathnameFileSystemNative> pathnameFileSystemNative;
    };

}

#endif
