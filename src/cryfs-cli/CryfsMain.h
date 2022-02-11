#ifndef EDS3_CRYFSMAIN_H
#define EDS3_CRYFSMAIN_H

#include <jni.h>
#include <string>
#include <FuseEds.h>

namespace cryfs_cli {

    void setCryfsConfig(jobject pathnameFileSystem, const std::string &configGroupPathname);

    FuseSession *openFuseSession(jobject pathnameFileSystem, const std::string &groupPathname);
};

#endif
