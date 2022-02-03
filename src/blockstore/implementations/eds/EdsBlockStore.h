#pragma once
#ifndef BLOCKSTORE_IMPLEMENTATIONS_EDS3_H_
#define BLOCKSTORE_IMPLEMENTATIONS_EDS3_H_

#include "../../interface/BlockStore2.h"
#include <cpp-utils/macros.h>
#include <cpp-utils/pointer/unique_ref.h>
#include <cpp-utils/logging/logging.h>
#include <pathresolver/PathResolverNative.h>
#include <filesystem/FileSystemNative.h>

namespace blockstore {
    namespace eds {

        class EdsBlockStore final : public BlockStore2 {
        public:
            EdsBlockStore(jobject pathnameFileSystem, const boost::filesystem::path &path);

            bool tryCreate(const BlockId &blockId, const cpputils::Data &data) override;

            bool remove(const BlockId &blockId) override;

            boost::optional<cpputils::Data> load(const BlockId &blockId) const override;

            void store(const BlockId &blockId, const cpputils::Data &data) override;

            uint64_t numBlocks() const override;

            uint64_t estimateNumFreeBytes() const override;

            uint64_t blockSizeFromPhysicalBlockSize(uint64_t blockSize) const override;

            void forEachBlock(std::function<void(const BlockId &)> callback) const override;

        private:
            jobject pathnameFileSystem;
            std::shared_ptr<FileSystemNative> fileSystem = nullptr;

            std::string rootGroupId;

            static const std::string FORMAT_VERSION_HEADER_PREFIX;
            static const std::string FORMAT_VERSION_HEADER;

//
            std::pair<std::string, std::string> getGroupAndFileNames(const BlockId &blockId) const;

//            static cpputils::Data _checkAndRemoveHeader(const cpputils::Data &data);
//            static bool _isAcceptedCryfsHeader(const cpputils::Data &data);
//            static bool _isOtherCryfsHeader(const cpputils::Data &data);
            static unsigned int formatVersionHeaderSize();

            DISALLOW_COPY_AND_ASSIGN(EdsBlockStore);
        };

    }
}

#endif
