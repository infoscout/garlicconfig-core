//
// Created by Peyman Mortazavi on 9/21/18.
//

#include <fstream>
#include <sstream>

#include <boost/filesystem.hpp>

#include "exceptions.h"
#include "repository.h"


using namespace std;

using namespace garlic;

namespace fs = boost::filesystem;


set<string> FileConfigRepository::list_configs() const {
  set<string> all_configs;
  auto end = fs::directory_iterator{};
  for (auto it = fs::directory_iterator{this->root_path}; it != end; it++) {
    if (not is_regular_file(it->status()) or it->path().extension() != ".garlic") {
      continue;
    }
    all_configs.insert(it->path().stem().string());
  }
  return all_configs;
}

void FileConfigRepository::save(const string& name, function<void(ostream &)> write_func) {
  auto file_path = fs::path{this->root_path} / name;
  ofstream output_stream;
  output_stream.open(file_path.string() + ".garlic", fstream::out | fstream::binary);
  write_func(output_stream);
  // Note that there is no need to close the output_stream here because it'll go out of scope and get closed on its own.
}

unique_ptr<istream> FileConfigRepository::retrieve(const string &name) const {
  auto file_path = fs::path{this->root_path} / (name + ".garlic");
  if (not fs::exists(file_path)) {
    throw ConfigNotFound(name);
  }
  auto input_stream = unique_ptr<ifstream>(new ifstream());
  input_stream->open(file_path.string(), fstream::in | fstream::binary);
  return std::move(input_stream);
}


unique_ptr<istream> MemoryConfigRepository::retrieve(const string &name) const {
  auto record_it = this->storage.find(name);
  if (record_it == this->storage.end()) {
    throw ConfigNotFound(name);
  }
  return unique_ptr<istringstream>(new istringstream(record_it->second));
}

set<string> MemoryConfigRepository::list_configs() const {
  set<string> keys;
  for(const auto& record : this->storage) {
    keys.insert(record.first);
  }
  return keys;
}

void MemoryConfigRepository::save(const string &name, function<void(ostream &)> write_func) {
  ostringstream output_stream;
  write_func(output_stream);
  this->storage[name] = output_stream.str();
}
