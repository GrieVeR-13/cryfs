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

using std::string;
using boost::optional;
using boost::none;
using cpputils::Data;

namespace blockstore {
    namespace eds {

        EdsBlockStore::EdsBlockStore(jobject pathnameFileSystem, const boost::filesystem::path& path) :
                pathnameFileSystem(pathnameFileSystem), _rootDir(path) {
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
            if (pathnameFileSystem.exists(filepath.string())) {
                return false;
            }

            store(blockId, data);
            return true;
        }

        bool EdsBlockStore::remove(const BlockId &blockId) {
            auto filepath = _getFilepath(blockId);
            auto fsObject = pathnameFileSystem.getObject(filepath.string());
            FsObjectNative fileSystemObject(fsObject);
            get_env()->DeleteLocalRef(fsObject);
            if (!fileSystemObject.isFile()) { // TODO Is this branch necessary?
                return false;
            }
            /*bool retval =*/ pathnameFileSystem.deleteObject(filepath.string());
//            if (!retval) {
//                cpputils::logging::LOG(cpputils::logging::ERR, "Couldn't find block {} to remove", blockId.ToString());
//                return false;
//            }
//            if (boost::filesystem::is_empty(filepath.parent_path())) {
//                boost::filesystem::remove(filepath.parent_path());
//            }
            pathnameFileSystem.deleteObject(filepath.parent_path().string());
            return true;
        }

        optional<Data> EdsBlockStore::load(const BlockId &blockId) const {
            auto fileContent = Data::LoadFromFile(_getFilepath(blockId));
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
            pathnameFileSystem.newGroup(filepath.parent_path().string(), false); // TODO Instead create all of them once at fs creation time?
            fileContent.StoreToFile(filepath);
        }

        uint64_t EdsBlockStore::numBlocks() const {
            uint64_t count = 0;
            auto env = get_env();
            auto fsoArray = pathnameFileSystem.listMembers(_rootDir.string());
            jsize numFiles = env->GetArrayLength(fsoArray);
            for (int i = 0; i < numFiles; i++) {
                jobject fso = env->GetObjectArrayElement(fsoArray, i);
                FsObjectNative fsObjectNative(fso);
                if (fsObjectNative.isGroup()) {
                    count += pathnameFileSystem.getNumberOfGroupMembers((_rootDir / fsObjectNative.getName()).string()); //todo check
                }
                env->DeleteLocalRef(fso);
            }
            env->DeleteLocalRef(fsoArray);
            return count;
        }

        uint64_t EdsBlockStore::estimateNumFreeBytes() const {
            auto env = get_env();
            auto spaceInfoObject = pathnameFileSystem.getSpaceInfo(_rootDir.string());
            SpaceInfoNative spaceInfoNative(env, spaceInfoObject);
            auto freeSpace = spaceInfoNative.getFreeSpace();
            env->DeleteLocalRef(spaceInfoObject);
            return freeSpace;
        }

        uint64_t EdsBlockStore::blockSizeFromPhysicalBlockSize(uint64_t blockSize) const {
            if (blockSize <= formatVersionHeaderSize()) {
                return 0;
            }
            return blockSize - formatVersionHeaderSize();
        }

        void EdsBlockStore::forEachBlock(std::function<void(const BlockId &)> callback) const {
            auto env = get_env();
            auto prefixFsoArray = pathnameFileSystem.listMembers(_rootDir.string());
            jsize numPrefixes = env->GetArrayLength(prefixFsoArray);
            for (int i = 0; i < numPrefixes; i++) {
                jobject prefixObject = env->GetObjectArrayElement(prefixFsoArray, i);
                FsObjectNative prefixFsObjectNative(prefixObject);
                if (!prefixFsObjectNative.isGroup())
                    continue;

                std::string blockIdPrefix = prefixFsObjectNative.getName();
                if (blockIdPrefix.size() != PREFIX_LENGTH || std::string::npos != blockIdPrefix.find_first_not_of(ALLOWED_BLOCKID_CHARACTERS)) {
                    // directory has wrong length or an invalid character
                    continue;
                }
                auto blockArray = pathnameFileSystem.listMembers((_rootDir / prefixFsObjectNative.getName()).string());
                jsize numBlocks = env->GetArrayLength(blockArray);
                for (int j = 0; j < numBlocks; j++) {
                    jobject blockObject = env->GetObjectArrayElement(blockArray, j);
                    FsObjectNative blockFsObjectNative(blockObject);

                    std::string blockIdPostfix = blockFsObjectNative.getName();
                    if (blockIdPostfix.size() != POSTFIX_LENGTH || std::string::npos != blockIdPostfix.find_first_not_of(ALLOWED_BLOCKID_CHARACTERS)) {
                        // filename has wrong length or an invalid character
                        continue;
                    }

                    callback(BlockId::FromString(blockIdPrefix + blockIdPostfix));

                    env->DeleteLocalRef(blockObject);
                }
                env->DeleteLocalRef(blockArray);

                env->DeleteLocalRef(prefixObject);
            }
            env->DeleteLocalRef(prefixFsoArray);
        }
    }
}
