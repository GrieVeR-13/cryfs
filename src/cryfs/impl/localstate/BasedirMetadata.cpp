#include "BasedirMetadata.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <vendor_cryptopp/sha.h>
#include <boost/filesystem/operations.hpp>
#include "LocalStateDir.h"
#include <cpp-utils/logging/logging.h>

namespace bf = boost::filesystem;
using boost::property_tree::ptree;
using boost::property_tree::write_json;
using boost::property_tree::read_json;
using boost::none;
using std::ostream;
using std::istream;
using std::ifstream;
using std::ofstream;
using std::string;
using namespace cpputils::logging;

namespace cryfs {

namespace {

ptree _load(const cpputils::FsAndPath &metadataFilePath) {
	try {
		ptree result;

		ifstream file(metadataFilePath.getPath().string()); //todoe stream
		if (file.good()) {
			read_json(file, result);
		}

		return result;
	}
	catch (...) {
		LOG(ERR, "Error loading BasedirMetadata");
		throw;
	}
}

void _save(const cpputils::FsAndPath &metadataFilePath, const ptree& data) {
  ofstream file(metadataFilePath.getPath().string(), std::ios::trunc);
  write_json(file, data);
}

string jsonPathForBasedir(const cpputils::FsAndPath &basedir) {
  return bf::canonical(basedir.getPath()).string() + ".filesystemId"; //todoe
}

}

BasedirMetadata::BasedirMetadata(ptree data, cpputils::FsAndPath filename)
  :_filename(std::move(filename)), _data(std::move(data)) {}

BasedirMetadata BasedirMetadata::load(const LocalStateDir& localStateDir) {
  auto filename = localStateDir.forBasedirMetadata();
  auto loaded = _load(filename);
  return BasedirMetadata(std::move(loaded), std::move(filename));
}

void BasedirMetadata::save() {
  _save(_filename, _data);
}

bool BasedirMetadata::filesystemIdForBasedirIsCorrect(const cpputils::FsAndPath &basedir, const CryConfig::FilesystemID &filesystemId) const {
  auto entry = _data.get_optional<string>(jsonPathForBasedir(basedir));
  if (entry == boost::none) {
    return true; // Basedir not known in local state yet.
  }
  auto filesystemIdFromState = CryConfig::FilesystemID::FromString(*entry);
  return filesystemIdFromState == filesystemId;
}

BasedirMetadata& BasedirMetadata::updateFilesystemIdForBasedir(const cpputils::FsAndPath &basedir, const CryConfig::FilesystemID &filesystemId) {
  _data.put<string>(jsonPathForBasedir(basedir), filesystemId.ToString());
  return *this;
}

}
