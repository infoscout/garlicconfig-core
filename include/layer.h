//
// Created by Peyman Mortazavi on 9/23/18.
//

#ifndef GARLICCONFIG_LAYER_H
#define GARLICCONFIG_LAYER_H

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

#include "exceptions.h"


namespace garlic {

  // TODO: Correct the plurality of the iterators.
  // TODO: get_array() and get_list() should return range structures.
  class LayerValue {
  public:

    using const_array_iterator = typename std::vector<std::shared_ptr<LayerValue>>::const_iterator;
    using const_member_iterator = typename std::map<std::string, std::shared_ptr<LayerValue>>::const_iterator;

    virtual bool is_string() const { return false; };
    virtual bool is_int() const { return false; };
    virtual bool is_bool() const { return false; };
    virtual bool is_double() const { return false; };
    virtual bool is_array() const { return false; };
    virtual bool is_object() const { return false; };

    virtual const std::string& get_string() const { throw TypeError(); }
    virtual const int& get_int() const { throw TypeError(); }
    virtual const bool& get_bool() const { throw TypeError(); }
    virtual const double& get_double() const { throw TypeError(); }

    // object-specific methods.
    virtual void set(const std::string& key, std::shared_ptr<LayerValue>&& value) { throw TypeError(); }
    virtual void set(const std::string& key, const std::shared_ptr<LayerValue>& value) { throw TypeError(); }
    virtual void set(const std::string& key, std::string&& value) { throw TypeError(); }
    virtual void set(const std::string& key, int&& value) { throw TypeError(); }

    virtual const std::shared_ptr<LayerValue>& get(const std::string& key) const  { throw TypeError(); }
    inline const std::shared_ptr<LayerValue>& operator[](const std::string& key) const { return this->get(key); }

    virtual const_member_iterator begin_member() const { throw TypeError(); }
    virtual const_member_iterator end_member() const { throw TypeError(); }

    virtual const std::shared_ptr<LayerValue>& resolve(const std::string& path) const { throw TypeError(); }

    // array-specific methods.
    virtual void add(const std::shared_ptr<LayerValue>& value) { throw TypeError(); }
    virtual void add(std::shared_ptr<LayerValue>&& value) { throw TypeError(); }
    virtual void add(std::string&& value) { throw TypeError(); }
    virtual void add(int&& value) { throw TypeError(); }

    virtual void remove(uint index) { throw TypeError(); }

    virtual const std::shared_ptr<LayerValue>& operator[](uint index) const { throw TypeError(); }

    virtual const_array_iterator begin_element() const { throw TypeError(); }
    virtual const_array_iterator end_element() const { throw TypeError(); }

    // utility

  };

  static std::shared_ptr<LayerValue> NotFoundPtr = nullptr;  // Just so that we have a reference to a NULL pointer.

  class StringValue : public LayerValue {
  public:
    explicit StringValue(std::string value) : value(std::move(value)) {}

    inline bool is_string() const override { return true; }
    const std::string& get_string() const override { return this->value; }

  private:
    std::string value;
  };

  class IntegerValue : public LayerValue {
  public:
    explicit IntegerValue(int value) : value(std::move(value)) {}

    inline bool is_int() const override { return true; }
    const int& get_int() const override { return this->value; }

  private:
    int value;
  };

  class BoolValue : public LayerValue {
  public:
    explicit BoolValue(bool value) : value(std::move(value)) {}

    inline bool is_bool() const override { return true; }
    const bool& get_bool() const override { return this->value; }

  private:
    bool value;
  };

  class DoubleValue : public LayerValue {
  public:
    explicit DoubleValue(double value) : value(std::move(value)) {}

    inline bool is_double() const override { return true; }
    const double& get_double() const override { return this->value; }

  private:
    double value;
  };

  class ObjectValue : public LayerValue {  // TODO: Use typedef for the iterator type.
  public:
    inline bool is_object() const override { return true; }

    const std::shared_ptr<LayerValue>& get(const std::string& key) const override {
      auto finder = this->property_list.find(key);
      if (finder != this->property_list.end()) {
        return finder->second;
      }
      return NotFoundPtr;
    }

    void set(const std::string& key, std::shared_ptr<LayerValue>&& value) override {
      this->property_list[key] = std::move(value);
    }

    void set(const std::string& key, const std::shared_ptr<LayerValue>& value) override {
      this->property_list[key] = value;
    }

    void set(const std::string& key, std::string&& value) override {
      this->set(key, std::make_shared<StringValue>(std::move(value)));
    }

    void set(const std::string& key, int&& value) override {
      this->set(key, std::make_shared<IntegerValue>(value));
    }

    const_member_iterator begin_member() const override {
      return this->property_list.begin();
    };

    const_member_iterator end_member() const override {
      return this->property_list.end();
    };

    const std::shared_ptr<LayerValue>& resolve(const std::string& path) const override {
      std::vector<std::string> parts;
      boost::split(parts, path, boost::is_any_of("."));
      const std::shared_ptr<LayerValue>* layer_value = &NotFoundPtr;
      const LayerValue* root = this;
      try {
        for (const auto &part : parts) {
          if (const auto &result = root->get(part)) {
            layer_value = &result;
            root = &*result;
          } else { return NotFoundPtr; }
        }
      } catch (const TypeError& error) { return NotFoundPtr; }
      return *layer_value;
    }

  private:
    std::map<std::string, std::shared_ptr<LayerValue>> property_list;
  };

  class ListValue : public LayerValue {
  public:
    bool is_array() const override { return true; };

    void add(const std::shared_ptr<LayerValue>& value) override { this->elements.push_back(value); }
    void add(std::shared_ptr<LayerValue>&& value) override { this->elements.push_back(std::move(value)); }
    void add(std::string&& value) override { this->elements.push_back(std::make_shared<StringValue>(std::move(value))); }
    void add(int&& value) override { this->elements.push_back(std::make_shared<IntegerValue>(value)); }

    void remove(uint index) override { this->elements.erase(this->elements.begin() + index); }

    virtual const std::shared_ptr<LayerValue>& operator[](uint index) const override { return this->elements[index]; }

    const_array_iterator begin_element() const override { return this->elements.begin(); }
    const_array_iterator end_element() const override { return this->elements.end(); }

  private:
    std::vector<std::shared_ptr<LayerValue>> elements;
  };
}

#endif //GARLICCONFIG_LAYER_H
