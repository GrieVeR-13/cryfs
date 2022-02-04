#include "FsAndPath.h"

namespace cpputils {

    FsAndPath::FsAndPath(std::shared_ptr<DataFileSystem> dataFileSystem, const boost::filesystem::path &path) :
            dataFileSystem(dataFileSystem), path(path) {}

}
