//
// Created by Peyman Mortazavi on 9/22/18.
//

#ifndef GARLICCONFIG_EXCEPTIONS_H
#define GARLICCONFIG_EXCEPTIONS_H

#include <exception>
#include <iostream>
#include <string>


namespace garlic {

  class ConfigNotFound : public std::exception {
  public:
    explicit ConfigNotFound(std::string config_name) : missing_config_name(std::move(config_name)) {}
    inline const std::string& config_name() const { return this->missing_config_name; }

  private:
    std::string missing_config_name;
  };

}

#endif //GARLICCONFIG_EXCEPTIONS_H
