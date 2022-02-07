#ifndef EDS3_CRYFSLAUNCHER_H
#define EDS3_CRYFSLAUNCHER_H

#include <jni.h>

namespace cryfs_cli {

    void setCryfsConfig(jobject pathnameFileSystem, const char *configGroupPathname);

    int openVolume(jobject pathnameFileSystem, const char *groupPathname);
};

#endif
