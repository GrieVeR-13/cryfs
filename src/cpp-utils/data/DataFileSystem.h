#ifndef EDS3_DATAFILESYSTEM_H
#define EDS3_DATAFILESYSTEM_H

#include <boost/filesystem.hpp>

namespace cpputils {

    class DataFileSystem {

    public:
        DataFileSystem() {}

        virtual ~DataFileSystem() {}

        bool exists(const boost::filesystem::path &path) const {
            return boost::filesystem::exists(path);
        }

        void create_directories(const boost::filesystem::path &path) const {
            boost::filesystem::create_directories(path);
        }
    };
}

#endif
