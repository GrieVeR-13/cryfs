#include "EdsBlockStore.h"
#include <log.h>
#include <filesystem/FileSystemNative.h>
#include <pathresolver/PathResolverNative.h>
#include <pathresolver/FsAndObjectNative.h>
#include <pathresolver/PathResolverProviderNative.h>
#include <util.h>
#include <filesystem/FsObjectNative.h>
#include <filesystem/SpaceInfoNative.h>
#include <Exception.h>
#include <cpp-utils/data/DataFileSystem.h>
#include <cpp-utils/data/EdsDataFileSystem.h>
#include "nativehelper/ScopedLocalRef.h"

using std::string;
using boost::optional;
using boost::none;
using cpputils::Data;

namespace blockstore {
    namespace eds {

        EdsBlockStore::EdsBlockStore(const cpputils::FsAndPath& path) :
                pathnameFileSystemNative(dynamic_cast<const cpputils::EdsDataFileSystem &> (*path.getDataFileSystem()).getPathnameFileSystemNative()), _rootDir(path.getPath()) {
        }


        const string EdsBlockStore::FORMAT_VERSION_HEADER_PREFIX = "cryfs;block;";
        const string EdsBlockStore::FORMAT_VERSION_HEADER = EdsBlockStore::FORMAT_VERSION_HEADER_PREFIX + "0";
        namespace {
            constexpr size_t PREFIX_LENGTH = 3;
            constexpr size_t POSTFIX_LENGTH = BlockId::STRING_LENGTH - PREFIX_LENGTH;
            constexpr const char *ALLOWED_BLOCKID_CHARACTERS = "0123456789ABCDEF";
        }

        boost::filesystem::path EdsBlockStore::_getFilepath(const BlockId &blockId) const {
            std::string blockIdStr = blockId.ToString();
            return _rootDir / blockIdStr.substr(0, PREFIX_LENGTH) / blockIdStr.substr(PREFIX_LENGTH);
        }

        Data EdsBlockStore::_checkAndRemoveHeader(const Data &data) {
            if (!_isAcceptedCryfsHeader(data)) {
                if (_isOtherCryfsHeader(data)) {
                    throw std::runtime_error(
                            "This block is not supported yet. Maybe it was created with a newer version of CryFS?");
                } else {
                    throw std::runtime_error("This is not a valid block.");
                }
            }
            Data result(data.size() - formatVersionHeaderSize());
            std::memcpy(result.data(), data.dataOffset(formatVersionHeaderSize()), result.size());
            return result;
        }

        bool EdsBlockStore::_isAcceptedCryfsHeader(const Data &data) {
            return 0 == std::memcmp(data.data(), FORMAT_VERSION_HEADER.c_str(), formatVersionHeaderSize());
        }

        bool EdsBlockStore::_isOtherCryfsHeader(const Data &data) {
            return 0 == std::memcmp(data.data(), FORMAT_VERSION_HEADER_PREFIX.c_str(), FORMAT_VERSION_HEADER_PREFIX.size());
        }

        unsigned int EdsBlockStore::formatVersionHeaderSize() {
            return FORMAT_VERSION_HEADER.size() + 1; // +1 because of the null byte
        }


        bool EdsBlockStore::tryCreate(const BlockId &blockId, const Data &data) {
            auto filepath = _getFilepath(blockId);
            if (pathnameFileSystemNative->exists(filepath.string())) {
                return false;
            }

            store(blockId, data);
            return true;
        }

        bool EdsBlockStore::remove(const BlockId &blockId) {
            auto filepath = _getFilepath(blockId);
            ScopedLocalRef<jobject> fsObject(getEnv(), pathnameFileSystemNative->getObject(filepath.string()));
            FsObjectNative fileSystemObject(fsObject.get());
            if (!fileSystemObject.isFile()) { // TODO Is this branch necessary?
                return false;
            }
            /*bool retval =*/ pathnameFileSystemNative->deleteObject(filepath.string());
//            if (!retval) {
//                cpputils::logging::LOG(cpputils::logging::ERR, "Couldn't find block {} to remove", blockId.ToString());
//                return false;
//            }
//            if (boost::filesystem::is_empty(filepath.parent_path())) {
//                boost::filesystem::remove(filepath.parent_path());
//            }
            pathnameFileSystemNative->deleteObject(filepath.parent_path().string());
            return true;
        }

        optional<Data> EdsBlockStore::load(const BlockId &blockId) const {
            auto fsAndPath = cpputils::FsAndPath(std::make_shared<cpputils::EdsDataFileSystem>(pathnameFileSystemNative), _getFilepath(blockId));
            auto fileContent = Data::LoadFromFile(fsAndPath);
            if (fileContent == none) {
                return boost::none;
            }
            return _checkAndRemoveHeader(*fileContent);
        }

        void EdsBlockStore::store(const BlockId &blockId, const Data &data) {
            Data fileContent(formatVersionHeaderSize() + data.size());
            std::memcpy(fileContent.data(), FORMAT_VERSION_HEADER.c_str(), formatVersionHeaderSize());
            std::memcpy(fileContent.dataOffset(formatVersionHeaderSize()), data.data(), data.size());
            auto filepath = _getFilepath(blockId);
            pathnameFileSystemNative->newGroup(filepath.parent_path().string(), false); // TODO Instead create all of them once at fs creation time?
            auto fsAndPath = cpputils::FsAndPath(std::make_shared<cpputils::EdsDataFileSystem>(pathnameFileSystemNative), filepath);
            fileContent.StoreToFile(fsAndPath);
        }

        uint64_t EdsBlockStore::numBlocks() const {
            uint64_t count = 0;
            auto env = getEnv();
            ScopedLocalRef<jobjectArray> fsoArray(env, pathnameFileSystemNative->listMembers(_rootDir.string()));
            jsize numFiles = env->GetArrayLength(fsoArray.get());
            for (int i = 0; i < numFiles; i++) {
                ScopedLocalRef<jobject> fso(env, env->GetObjectArrayElement(fsoArray.get(), i));
                FsObjectNative fsObjectNative(fso.get());
                if (fsObjectNative.isGroup()) {
                    count += pathnameFileSystemNative->getNumberOfGroupMembers((_rootDir / fsObjectNative.getName()).string()); //todo check
                }
            }
            return count;
        }

        uint64_t EdsBlockStore::estimateNumFreeBytes() const {
            auto env = getEnv();
            ScopedLocalRef<jobject> spaceInfoObject(env, pathnameFileSystemNative->getSpaceInfoObject(_rootDir.string()));
            SpaceInfoNative spaceInfoNative(env, spaceInfoObject.get());
            auto freeSpace = spaceInfoNative.getFreeSpace();
            return freeSpace;
        }

        uint64_t EdsBlockStore::blockSizeFromPhysicalBlockSize(uint64_t blockSize) const {
            if (blockSize <= formatVersionHeaderSize()) {
                return 0;
            }
            return blockSize - formatVersionHeaderSize();
        }

        void EdsBlockStore::forEachBlock(std::function<void(const BlockId &)> callback) const {
            auto env = getEnv();
            ScopedLocalRef<jobjectArray> prefixFsoArray(env, pathnameFileSystemNative->listMembers(_rootDir.string()));
            jsize numPrefixes = env->GetArrayLength(prefixFsoArray.get());
            for (int i = 0; i < numPrefixes; i++) {
                ScopedLocalRef<jobject> prefixObject(env, env->GetObjectArrayElement(prefixFsoArray.get(), i));
                FsObjectNative prefixFsObjectNative(prefixObject.get());
                if (!prefixFsObjectNative.isGroup())
                    continue;

                std::string blockIdPrefix = prefixFsObjectNative.getName();
                if (blockIdPrefix.size() != PREFIX_LENGTH || std::string::npos != blockIdPrefix.find_first_not_of(ALLOWED_BLOCKID_CHARACTERS)) {
                    // directory has wrong length or an invalid character
                    continue;
                }
                ScopedLocalRef<jobjectArray> blockArray(env, pathnameFileSystemNative->listMembers((_rootDir / prefixFsObjectNative.getName()).string()));
                jsize numBlocks = env->GetArrayLength(blockArray.get());
                for (int j = 0; j < numBlocks; j++) {
                    ScopedLocalRef<jobject> blockObject(env, env->GetObjectArrayElement(blockArray.get(), j));
                    FsObjectNative blockFsObjectNative(blockObject.get());

                    std::string blockIdPostfix = blockFsObjectNative.getName();
                    if (blockIdPostfix.size() != POSTFIX_LENGTH || std::string::npos != blockIdPostfix.find_first_not_of(ALLOWED_BLOCKID_CHARACTERS)) {
                        // filename has wrong length or an invalid character
                        continue;
                    }

                    callback(BlockId::FromString(blockIdPrefix + blockIdPostfix));

                }
            }
        }
    }
}
