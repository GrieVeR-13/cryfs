project (cryfs)

#include(utils.cmake)

set(LIB_SOURCES
#        RunFileSystem.cpp

        #    cryfs.cpp
        impl/config/Testtt.cpp
        impl/CryfsException.cpp
        impl/config/crypto/outer/OuterConfig.cpp
        impl/config/crypto/outer/OuterEncryptor.cpp
        impl/config/crypto/CryConfigEncryptorFactory.cpp
        impl/config/crypto/inner/ConcreteInnerEncryptor.cpp
        impl/config/crypto/inner/InnerConfig.cpp
        impl/config/crypto/inner/InnerEncryptor.cpp
        impl/config/crypto/CryConfigEncryptor.cpp
        impl/config/CryConfigConsole.cpp
        impl/config/CryConfigLoader.cpp
        impl/config/CryConfig.cpp
        impl/config/CryConfigFile.cpp
        impl/config/CryCipher.cpp
        impl/config/CryConfigCreator.cpp
        impl/config/CryKeyProvider.cpp
        impl/config/CryPasswordBasedKeyProvider.cpp
        impl/config/CryPresetPasswordBasedKeyProvider.cpp
        impl/filesystem/CryOpenFile.cpp
        impl/filesystem/fsblobstore/utils/DirEntry.cpp
        impl/filesystem/fsblobstore/utils/DirEntryList.cpp
        impl/filesystem/fsblobstore/FsBlobStore.cpp
        impl/filesystem/fsblobstore/FsBlobView.cpp
        impl/filesystem/fsblobstore/FileBlob.cpp
        impl/filesystem/fsblobstore/FsBlob.cpp
        impl/filesystem/fsblobstore/SymlinkBlob.cpp
        impl/filesystem/fsblobstore/DirBlob.cpp
        impl/filesystem/CryNode.cpp
        impl/filesystem/parallelaccessfsblobstore/DirBlobRef.cpp
        impl/filesystem/parallelaccessfsblobstore/ParallelAccessFsBlobStore.cpp
        impl/filesystem/parallelaccessfsblobstore/ParallelAccessFsBlobStoreAdapter.cpp
        impl/filesystem/parallelaccessfsblobstore/FsBlobRef.cpp
        impl/filesystem/parallelaccessfsblobstore/FileBlobRef.cpp
        impl/filesystem/parallelaccessfsblobstore/SymlinkBlobRef.cpp
        impl/filesystem/CrySymlink.cpp
        impl/filesystem/CryDir.cpp
        impl/filesystem/cachingfsblobstore/DirBlobRef.cpp
        impl/filesystem/cachingfsblobstore/CachingFsBlobStore.cpp
        impl/filesystem/cachingfsblobstore/FsBlobRef.cpp
        impl/filesystem/cachingfsblobstore/FileBlobRef.cpp
        impl/filesystem/cachingfsblobstore/SymlinkBlobRef.cpp
        impl/filesystem/CryFile.cpp
        impl/filesystem/CryDevice.cpp
        impl/localstate/LocalStateDir.cpp
        impl/localstate/LocalStateMetadata.cpp
        impl/localstate/BasedirMetadata.cpp
        )

add_library(cryfs STATIC ${LIB_SOURCES})
target_link_libraries(cryfs PUBLIC cpp-utils blockstore blobstore)
#target_link_libraries(cryfs PUBLIC cpp-utils fspp-interface blockstore blobstore gitversion)
target_add_boost(cryfs) # TODO Check that dependent projects don't get boost added (use PRIVATE here)
target_enable_style_warnings(cryfs)
target_activate_cpp14(cryfs)

#install(TARGETS cryfs
#        DESTINATION lib
#        CONFIGURATIONS Release
#)

