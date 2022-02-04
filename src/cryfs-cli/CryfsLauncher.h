#ifndef EDS3_CRYFSLAUNCHER_H
#define EDS3_CRYFSLAUNCHER_H

#include <jni.h>

namespace cryfs_cli {
    namespace eds {
        int openVolume(jobject pathnameFileSystem, const char* groupPathname);
    }
};

#endif
