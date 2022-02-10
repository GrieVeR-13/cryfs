#ifndef EDS3_CRYFSMAIN_H
#define EDS3_CRYFSMAIN_H

#include <jni.h>
#include <string>
#include <FuseFileSystemNative.h>

namespace cryfs_cli {

    void setCryfsConfig(jobject pathnameFileSystem, const std::string &configGroupPathname);

    FuseFileSystemNative *openFuseFileSystemNative(jobject pathnameFileSystem, const std::string &groupPathname);
};

#endif
