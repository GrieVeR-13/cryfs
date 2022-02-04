#pragma once
#ifndef MESSMER_CRYFSCLI_ENVIRONMENT_H
#define MESSMER_CRYFSCLI_ENVIRONMENT_H

#include <string>
#include <boost/filesystem/path.hpp>
#include <cpp-utils/data/FsAndPath.h>

namespace cryfs_cli {

    class Environment {
    public:
        static bool isNoninteractive();
        static bool noUpdateCheck();
        static cpputils::FsAndPath localStateDir();
        static const cpputils::FsAndPath& defaultLocalStateDir();

        static const std::string FRONTEND_KEY;
        static const std::string FRONTEND_NONINTERACTIVE;
        static const std::string NOUPDATECHECK_KEY;
        static const std::string LOCALSTATEDIR_KEY;

    private:
        Environment() = delete;

    };

}

#endif
