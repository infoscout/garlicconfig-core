//
// Created by Peyman Mortazavi on 9/21/18.
//

#include <fstream>
#include <sstream>

#include <tinydir/tinydir.h>

#include "exceptions.h"
#include "repository.h"


using namespace std;

using namespace garlic;


string path_append(const string& original, const string& name) {
  if (original[original.size()-1] == '/' && name[name.size()-1] == '/') {
    return original + name.substr(1);
  } else if (original[original.size()-1] != '/' && name[name.size()-1] != '/') {
    return original + "/" + name;
  } else {
    return original + name;
  }
}

set<string> FileConfigRepository::list_configs() const {
  tinydir_dir dir;
  tinydir_open(&dir, this->root_path.c_str());
  set<string> all_configs;
  while (dir.has_next) {
    tinydir_file file;
    tinydir_readfile(&dir, &file);
    if (file.is_reg && strcmp(file.extension, "garlic") == 0) {
      string tmp(file.name);
      all_configs.emplace(tmp.substr(0, tmp.find_last_of('.')));
    }
    tinydir_next(&dir);
  }
  tinydir_close(&dir);
  return all_configs;
}

void FileConfigRepository::save(const string& name, function<void(ostream &)> write_func) {
  auto file_path = path_append(this->root_path,  name + ".garlic");
  ofstream output_stream;
  output_stream.open(file_path, fstream::out | fstream::binary);
  write_func(output_stream);
  // Note that there is no need to close the output_stream here because it'll go out of scope and get closed on its own.
}

unique_ptr<istream> FileConfigRepository::retrieve(const string &name) const {
  auto file_path = path_append(this->root_path,  name + ".garlic");
  auto input_stream = unique_ptr<ifstream>(new ifstream());
  input_stream->open(file_path, fstream::in | fstream::binary);
  if (!input_stream->good()) {
    throw ConfigNotFound(name);
  }
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
