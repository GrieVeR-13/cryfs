#ifndef EDS3_FSANDPATH_H
#define EDS3_FSANDPATH_H

#include <boost/filesystem.hpp>
#include "DataFileSystem.h"

namespace cpputils {

    class FsAndPath {

    public:
        FsAndPath() {}

        FsAndPath(std::shared_ptr<DataFileSystem> dataFileSystem, const boost::filesystem::path &path);

        const std::shared_ptr<cpputils::DataFileSystem> &getDataFileSystem() const { //todoe check all for exception
            return dataFileSystem;
        }

        const boost::filesystem::path &getPath() const {
            return path;
        }


    private:
        std::shared_ptr<DataFileSystem> dataFileSystem;
        boost::filesystem::path path;
    };

    inline FsAndPath operator/(const FsAndPath &lhs, const boost::filesystem::path &rhs) {
        return FsAndPath(lhs.getDataFileSystem(), lhs.getPath() / rhs);
    }

}

#endif
