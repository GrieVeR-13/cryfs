#include "EdsBlockStore.h"

using std::string;
using boost::optional;
using boost::none;
using cpputils::Data;

namespace blockstore {
namespace eds {

EdsBlockStore::EdsBlockStore() {}

bool EdsBlockStore::tryCreate(const BlockId &blockId, const Data &data) {
  return true;
}

bool EdsBlockStore::remove(const BlockId &blockId) {
  return true;
}

optional<Data> EdsBlockStore::load(const BlockId &blockId) const {
//  return _checkAndRemoveHeader(*fileContent);
}

void EdsBlockStore::store(const BlockId &blockId, const Data &data) {
}

uint64_t EdsBlockStore::numBlocks() const {
  return count;
}

uint64_t EdsBlockStore::estimateNumFreeBytes() const {
}

uint64_t EdsBlockStore::blockSizeFromPhysicalBlockSize(uint64_t blockSize) const {
}

void EdsBlockStore::forEachBlock(std::function<void (const BlockId &)> callback) const {

}

}
}
