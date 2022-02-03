#include "EdsBlockStore.h"
#include <log.h>
#include <filesystem/FileSystemNative.h>
#include <pathresolver/PathResolverNative.h>
#include <pathresolver/FsAndObjectNative.h>
#include <pathresolver/PathResolverProviderNative.h>
#include <util.h>
#include <filesystem/FsObjectNative.h>
#include <Exception.h>

using std::string;
using boost::optional;
using boost::none;
using cpputils::Data;

namespace blockstore {
    namespace eds {

        EdsBlockStore::EdsBlockStore(jobject pathnameFileSystem, const boost::filesystem::path &path) :
                pathnameFileSystem(pathnameFileSystem) {
            auto pathResolver = PathResolverProviderNative::getInstance().getPathResolver();
            auto fsAndObject = FsAndObjectNative::resolvePathToFsAndObject(pathResolver, path.string());
            auto fsObject = fsAndObject.getFsObject();
            if (fsObject == nullptr)
                throw Exception();
            fileSystem = fsAndObject.getFileSystem();
            FsObjectNative fsObjectNative(fsObject);
            rootGroupId = fsObjectNative.getId();
        }


        const string EdsBlockStore::FORMAT_VERSION_HEADER_PREFIX = "cryfs;block;";
        const string EdsBlockStore::FORMAT_VERSION_HEADER = EdsBlockStore::FORMAT_VERSION_HEADER_PREFIX + "0";
        namespace {
            constexpr size_t PREFIX_LENGTH = 3;
//            constexpr size_t POSTFIX_LENGTH = BlockId::STRING_LENGTH - PREFIX_LENGTH;
//            constexpr const char *ALLOWED_BLOCKID_CHARACTERS = "0123456789ABCDEF";
        }

        std::pair<std::string, std::string> EdsBlockStore::getGroupAndFileNames(const BlockId &blockId) const {
            std::string blockIdStr = blockId.ToString();
            return std::pair<std::string, std::string>(blockIdStr.substr(0, PREFIX_LENGTH),
                                                       blockIdStr.substr(PREFIX_LENGTH));
        }

//        Data EdsBlockStore::_checkAndRemoveHeader(const Data &data) {
//            if (!_isAcceptedCryfsHeader(data)) {
//                if (_isOtherCryfsHeader(data)) {
//                    throw std::runtime_error(
//                            "This block is not supported yet. Maybe it was created with a newer version of CryFS?");
//                } else {
//                    throw std::runtime_error("This is not a valid block.");
//                }
//            }
//            Data result(data.size() - formatVersionHeaderSize());
//            std::memcpy(result.data(), data.dataOffset(formatVersionHeaderSize()), result.size());
//            return result;
//        }

//        bool EdsBlockStore::_isAcceptedCryfsHeader(const Data &data) {
//            return 0 == std::memcmp(data.data(), FORMAT_VERSION_HEADER.c_str(), formatVersionHeaderSize());
//        }
//
//        bool EdsBlockStore::_isOtherCryfsHeader(const Data &data) {
//            return 0 == std::memcmp(data.data(), FORMAT_VERSION_HEADER_PREFIX.c_str(), FORMAT_VERSION_HEADER_PREFIX.size());
//        }

        unsigned int EdsBlockStore::formatVersionHeaderSize() {
            return FORMAT_VERSION_HEADER.size() + 1; // +1 because of the null byte
        }


        bool EdsBlockStore::tryCreate(const BlockId &blockId, const Data &data) {
            auto filepath = getGroupAndFileNames(blockId);
//            if (boost::filesystem::exists(filepath)) {
//                return false; //todoe
//            }

            store(blockId, data);
            return true;
        }

        bool EdsBlockStore::remove(const BlockId &blockId) {
            auto filepath = getGroupAndFileNames(blockId);
//            if (!boost::filesystem::is_regular_file(filepath)) { // TODO Is this branch necessary?
//                return false;
//            }
//            bool retval = boost::filesystem::remove(filepath);
//            if (!retval) {
//                cpputils::logging::LOG(cpputils::logging::ERR, "Couldn't find block {} to remove", blockId.ToString());
//                return false;
//            }
//            if (boost::filesystem::is_empty(filepath.parent_path())) {
//                boost::filesystem::remove(filepath.parent_path());
//            }
            return true;
        }

        optional<Data> EdsBlockStore::load(const BlockId &blockId) const {
//            auto fileContent = Data::LoadFromFile(getGroupAndFileNames(blockId));
//            if (fileContent == none) {
//                return boost::none;
//            }
//            return _checkAndRemoveHeader(*fileContent);
        }

        void EdsBlockStore::store(const BlockId &blockId, const Data &data) {
            Data fileContent(formatVersionHeaderSize() + data.size());
            std::memcpy(fileContent.data(), FORMAT_VERSION_HEADER.c_str(), formatVersionHeaderSize());
            std::memcpy(fileContent.dataOffset(formatVersionHeaderSize()), data.data(), data.size());
            auto groupAndFileNames = getGroupAndFileNames(blockId);

            auto groupObject = fileSystem->newGroup(groupAndFileNames.first, rootGroupId);
            FsObjectNative group(groupObject);

            auto fileObject = fileSystem->getOrNewFile(groupAndFileNames.second, group.getId());
            auto pathResolver = PathResolverProviderNative::getInstance().getPathResolver();
//            auto fsAndObject = FsAndObjectNative::resolvePathToFsAndObject(pathResolver, path.string());


//            fileContent.StoreToFile(groupAndFileNames);
        }

        uint64_t EdsBlockStore::numBlocks() const {
//            uint64_t count = 0;
//            for (auto prefixDir = boost::filesystem::directory_iterator(_rootDir);
//                 prefixDir != boost::filesystem::directory_iterator(); ++prefixDir) {
//                if (boost::filesystem::is_directory(prefixDir->path())) {
//                    count += std::distance(boost::filesystem::directory_iterator(prefixDir->path()),
//                                           boost::filesystem::directory_iterator());
//                }
//            }
//            return count;
        }

        uint64_t EdsBlockStore::estimateNumFreeBytes() const {
//	return cpputils::free_disk_space_in_bytes(_rootDir);
            return 0;
        }

        uint64_t EdsBlockStore::blockSizeFromPhysicalBlockSize(uint64_t blockSize) const {
            if (blockSize <= formatVersionHeaderSize()) {
                return 0;
            }
            return blockSize - formatVersionHeaderSize();
        }

        void EdsBlockStore::forEachBlock(std::function<void(const BlockId &)> callback) const {
//            for (auto prefixDir = boost::filesystem::directory_iterator(_rootDir);
//                 prefixDir != boost::filesystem::directory_iterator(); ++prefixDir) {
//                if (!boost::filesystem::is_directory(prefixDir->path())) {
//                    continue;
//                }
//
//                std::string blockIdPrefix = prefixDir->path().filename().string();
//                if (blockIdPrefix.size() != PREFIX_LENGTH ||
//                    std::string::npos != blockIdPrefix.find_first_not_of(ALLOWED_BLOCKID_CHARACTERS)) {
//                    // directory has wrong length or an invalid character
//                    continue;
//                }
//
//                for (auto block = boost::filesystem::directory_iterator(prefixDir->path());
//                     block != boost::filesystem::directory_iterator(); ++block) {
//                    std::string blockIdPostfix = block->path().filename().string();
//                    if (blockIdPostfix.size() != POSTFIX_LENGTH ||
//                        std::string::npos != blockIdPostfix.find_first_not_of(ALLOWED_BLOCKID_CHARACTERS)) {
//                        // filename has wrong length or an invalid character
//                        continue;
//                    }
//
//                    callback(BlockId::FromString(blockIdPrefix + blockIdPostfix));
//                }
//            }
        }


    }
}

#pragma clang diagnostic pop