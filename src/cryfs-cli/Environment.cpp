#include "Environment.h"
#include <cryfs/impl/ErrorCodes.h>
#include <cstdlib>
//#include <cpp-utils/system/homedir.h>
#include <boost/filesystem.hpp>
#include <cpp-utils/data/FsAndPath.h>
#include <cryfs/impl/CryfsException.h>

using std::string;
namespace bf = boost::filesystem;

namespace cryfs_cli {
    const string Environment::FRONTEND_KEY = "CRYFS_FRONTEND";
    const string Environment::FRONTEND_NONINTERACTIVE = "noninteractive";
    const string Environment::NOUPDATECHECK_KEY = "CRYFS_NO_UPDATE_CHECK";
    const string Environment::LOCALSTATEDIR_KEY = "CRYFS_LOCAL_STATE_DIR";

    static cpputils::FsAndPath localStateDirPath;

    bool Environment::isNoninteractive() {
//        char *frontend = std::getenv(FRONTEND_KEY.c_str());
//        return frontend != nullptr && frontend == FRONTEND_NONINTERACTIVE;
        return false;
    }

    bool Environment::noUpdateCheck() {
        return nullptr != std::getenv(NOUPDATECHECK_KEY.c_str());
    }

    const boost::filesystem::path& Environment::defaultLocalStateDir() {
//        static const bf::path value = cpputils::system::HomeDirectory::getXDGDataDir() / "cryfs";
//        return value;
    }

    cpputils::FsAndPath Environment::localStateDir() {
//        const char* localStateDir = std::getenv(LOCALSTATEDIR_KEY.c_str());

//        if (nullptr == localStateDir) {
//            return defaultLocalStateDir();
//        }
//
//        return bf::absolute(localStateDir);
        if (localStateDirPath.getPath().empty())
            throw cryfs::CryfsException("Invalid local state dir path.", cryfs::ErrorCode::UnspecifiedError);
        return localStateDirPath;
    }

    void Environment::setConfigPath(const cpputils::FsAndPath &path) {
        localStateDirPath = path;
    }
}
