#include "CryDir.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#include <fspp/fuse/FuseErrnoException.h>
#include "CryDevice.h"
#include "CryFile.h"
#include "CryOpenFile.h"
#include <cpp-utils/system/time.h>
#include "fsblobstore/utils/TimestampUpdateBehavior.h"

//TODO Get rid of this in favor of exception hierarchy
using fspp::fuse::FuseErrnoException;

namespace bf = boost::filesystem;

using std::string;
using std::vector;

using blockstore::BlockId;
using cpputils::unique_ref;
using cpputils::make_unique_ref;
using cpputils::dynamic_pointer_move;
using boost::optional;
using boost::none;
using cryfs::fsblobstore::DirBlob;

namespace cryfs {

CryDir::CryDir(CryDevice *device, boost::filesystem::path path, optional<std::shared_ptr<DirBlob>> parent, optional<std::shared_ptr<DirBlob>> grandparent, const BlockId &blockId)
: CryNode(device, std::move(path), std::move(parent), std::move(grandparent), blockId) {
}

CryDir::~CryDir() {
}

unique_ref<fspp::OpenFile> CryDir::createAndOpenFile(const string &name, mode_t mode, uid_t uid, gid_t gid) {
  device()->callFsActionCallbacks();
  if (!isRootDir()) {
    //TODO Instead of doing nothing when we're the root directory, handle timestamps in the root dir correctly (and delete isRootDir() function)
    parent()->updateModificationTimestampForChild(blockId());
  }
  auto child = device()->CreateFileBlob(blockId());
  auto now = cpputils::time::now();
  auto dirBlob = LoadBlob();
  dirBlob->AddChildFile(name, child->blockId(), mode, uid, gid, now, now);
  return make_unique_ref<CryOpenFile>(device(), dirBlob->blockId(), child->blockId());
}

void CryDir::createDir(const string &name, mode_t mode, uid_t uid, gid_t gid) {
  device()->callFsActionCallbacks();
  if (!isRootDir()) {
    //TODO Instead of doing nothing when we're the root directory, handle timestamps in the root dir correctly (and delete isRootDir() function)
    parent()->updateModificationTimestampForChild(blockId());
  }
  auto blob = LoadBlob();
  auto child = device()->CreateDirBlob(blockId());
  auto now = cpputils::time::now();
  blob->AddChildDir(name, child->blockId(), mode, uid, gid, now, now);
}

unique_ref<DirBlob> CryDir::LoadBlob() const {
  auto blob = CryNode::LoadBlob();
  auto dir_blob = dynamic_pointer_move<DirBlob>(blob);
  ASSERT(dir_blob != none, "Blob does not store a directory");
  return std::move(*dir_blob);
}

unique_ref<vector<fspp::Dir::Entry>> CryDir::children() {
  device()->callFsActionCallbacks();
  if (!isRootDir()) { // NOLINT (workaround https://gcc.gnu.org/bugzilla/show_bug.cgi?id=82481 )
    //TODO Instead of doing nothing when we're the root directory, handle timestamps in the root dir correctly (and delete isRootDir() function)
    parent()->updateAccessTimestampForChild(blockId(), fsblobstore::TimestampUpdateBehavior::RELATIME);
  }
  auto children = make_unique_ref<vector<fspp::Dir::Entry>>();
  children->push_back(fspp::Dir::Entry(fspp::Dir::EntryType::DIR, "."));
  children->push_back(fspp::Dir::Entry(fspp::Dir::EntryType::DIR, ".."));
  auto blob = LoadBlob();
  blob->AppendChildrenTo(children.get());
  return children;
}

fspp::Dir::EntryType CryDir::getType() const {
  device()->callFsActionCallbacks();
  return fspp::Dir::EntryType::DIR;
}

void CryDir::createSymlink(const string &name, const bf::path &target, uid_t uid, gid_t gid) {
  device()->callFsActionCallbacks();
  if (!isRootDir()) {
    //TODO Instead of doing nothing when we're the root directory, handle timestamps in the root dir correctly (and delete isRootDir() function)
    parent()->updateModificationTimestampForChild(blockId());
  }
  auto blob = LoadBlob();
  auto child = device()->CreateSymlinkBlob(target, blockId());
  auto now = cpputils::time::now();
  blob->AddChildSymlink(name, child->blockId(), uid, gid, now, now);
}

void CryDir::remove() {
  device()->callFsActionCallbacks();
  if (grandparent() != none) {
    //TODO Instead of doing nothing when we're in the root directory, handle timestamps in the root dir correctly
    (*grandparent())->updateModificationTimestampForChild(parent()->blockId());
  }
  {
    auto blob = LoadBlob();
    if (0 != blob->NumChildren()) {
      throw FuseErrnoException(ENOTEMPTY);
    }
  }
  //TODO removeNode() calls CryDevice::RemoveBlob, which loads the blob again. So we're loading it twice. Should be optimized.
  removeNode();
}

}
