#ifndef EDS3_DATAFILESYSTEM_H
#define EDS3_DATAFILESYSTEM_H

#include <boost/filesystem.hpp>
#include "jni.h"

namespace cpputils {

    class DataFileSystem {

    public:
        DataFileSystem() {}

        virtual ~DataFileSystem() {}

        virtual bool exists(const boost::filesystem::path &path) const {
            return boost::filesystem::exists(path);
        }

        virtual void create_directories(const boost::filesystem::path &path) const {
            boost::filesystem::create_directories(path);
        }

        virtual std::unique_ptr<std::istream> openInputStream(const boost::filesystem::path &path) const {
            return std::make_unique<std::ifstream>(path.string());
        }

        virtual std::unique_ptr<std::ostream> openOutputStream(const boost::filesystem::path &path) const {
            return std::make_unique<std::ofstream>(path.string());
        }

    };
}

#endif
