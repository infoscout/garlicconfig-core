//
// Created by Peyman Mortazavi on 9/21/18.
//

#ifndef GARLICCONFIG_REPOSITORY_H
#define GARLICCONFIG_REPOSITORY_H

#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>


namespace garlic {

  class ConfigRepository {
  public:
    virtual std::set<std::string> list_configs() const = 0;
    virtual void save(const std::string& name, std::function<void (std::ostream&)> write_func) = 0;
    virtual std::unique_ptr<std::istream> retrieve(const std::string& name) const = 0;

    inline void save(const std::string& name, std::istream& input_stream) {
      this->save(name, [&input_stream](std::ostream& output_stream){
        output_stream << input_stream.rdbuf();
      });
    }
  };


  class FileConfigRepository : public ConfigRepository {
  public:
    explicit FileConfigRepository(std::string root_path) : root_path(std::move(root_path)) {}
    std::set<std::string> list_configs() const override;
    void save(const std::string& name, std::function<void (std::ostream&)> write_func) override;
    std::unique_ptr<std::istream> retrieve(const std::string& name) const override;

  protected:
    std::string root_path;
  };


  class MemoryRepository : public ConfigRepository {
  public:
    std::set<std::string> list_configs() const override;
    void save(const std::string& name, std::function<void (std::ostream&)> write_func) override;
    std::unique_ptr<std::istream> retrieve(const std::string& name) const override;

  private:
    std::map<std::string, std::string> storage;
  };

}

#endif //GARLICCONFIG_REPOSITORY_H
