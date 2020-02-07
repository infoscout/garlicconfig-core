//
// Created by Peyman Mortazavi on 9/23/18.
//

#ifndef GARLICCONFIG_LAYER_H
#define GARLICCONFIG_LAYER_H

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "exceptions.h"


namespace garlic {

  class LayerValue {
  public:

    using const_array_iterator = typename std::vector<std::shared_ptr<LayerValue>>::const_iterator;
    using const_member_iterator = typename std::map<std::string, std::shared_ptr<LayerValue>>::const_iterator;

    virtual std::shared_ptr<LayerValue> clone() const = 0;

    virtual bool is_string() const { return false; };
    virtual bool is_int() const { return false; };
    virtual bool is_bool() const { return false; };
    virtual bool is_double() const { return false; };
    virtual bool is_array() const { return false; };
    virtual bool is_object() const { return false; };
    virtual bool is_null() const { return false; }

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
    virtual void apply(const std::shared_ptr<LayerValue>& layer) { throw TypeError(); }

    // array-specific methods.
    virtual void add(const std::shared_ptr<LayerValue>& value) { throw TypeError(); }
    virtual void add(std::shared_ptr<LayerValue>&& value) { throw TypeError(); }
    virtual void add(std::string&& value) { throw TypeError(); }
    virtual void add(int&& value) { throw TypeError(); }

    virtual void remove(unsigned int index) { throw TypeError(); }

    virtual std::shared_ptr<LayerValue>& operator[](unsigned int index) { throw TypeError(); }

    virtual const_array_iterator begin_element() const { throw TypeError(); }
    virtual const_array_iterator end_element() const { throw TypeError(); }

  private:
    // Utility structures for supporting range loops.
    struct array_range {
      const LayerValue& value;
      explicit array_range(const LayerValue& value) : value(value) {}

      inline const_array_iterator begin() const { return value.begin_element(); }
      inline const_array_iterator end() const { return value.end_element(); }
    };

    struct member_range {
      const LayerValue &value;
      explicit member_range(const LayerValue& value) : value(value) {}

      inline const_member_iterator begin() const { return value.begin_member(); }
      inline const_member_iterator end() const { return value.end_member(); }
    };

  public:
    inline array_range get_array() const { return array_range(*this); }
    inline member_range get_object() const { return member_range(*this); }
  };

  static std::shared_ptr<LayerValue> NotFoundPtr = nullptr;  // Just so that we have a reference to a NULL pointer.

  class NullValue : public LayerValue {
  public:
    std::shared_ptr<LayerValue> clone() const override {
      return std::make_shared<NullValue>();
    }

    inline bool is_null() const override { return true; }
  };

  class StringValue : public LayerValue {
  public:
    explicit StringValue(std::string value) : value(std::move(value)) {}

    std::shared_ptr<LayerValue> clone() const override {
      return std::make_shared<StringValue>(this->value);
    }

    inline bool is_string() const override { return true; }
    const std::string& get_string() const override { return this->value; }

  private:
    std::string value;
  };

  class IntegerValue : public LayerValue {
  public:
    explicit IntegerValue(int value) : value(value) {}

    std::shared_ptr<LayerValue> clone() const override {
      return std::make_shared<IntegerValue>(this->value);
    }

    inline bool is_int() const override { return true; }
    const int& get_int() const override { return this->value; }

  private:
    int value;
  };

  class BoolValue : public LayerValue {
  public:
    explicit BoolValue(bool value) : value(value) {}

    std::shared_ptr<LayerValue> clone() const override {
      return std::make_shared<BoolValue>(this->value);
    }

    inline bool is_bool() const override { return true; }
    const bool& get_bool() const override { return this->value; }

  private:
    bool value;
  };

  class DoubleValue : public LayerValue {
  public:
    explicit DoubleValue(double value) : value(value) {}

    std::shared_ptr<LayerValue> clone() const override {
      return std::make_shared<DoubleValue>(this->value);
    }

    inline bool is_double() const override { return true; }
    const double& get_double() const override { return this->value; }

  private:
    double value;
  };

  class ObjectValue : public LayerValue {
  public:
    inline bool is_object() const override { return true; }

    std::shared_ptr<LayerValue> clone() const override;

    const std::shared_ptr<LayerValue>& get(const std::string& key) const override;

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

    const std::shared_ptr<LayerValue>& resolve(const std::string& path) const override;

    void apply(const std::shared_ptr<LayerValue>& layer) override;

  private:
    std::map<std::string, std::shared_ptr<LayerValue>> property_list;
  };


  class ListValue : public LayerValue {
  public:
    ListValue() = default;
    explicit ListValue(size_t size);

    inline bool is_array() const override { return true; }

    std::shared_ptr<LayerValue> clone() const override;

    void add(const std::shared_ptr<LayerValue>& value) override;
    void add(std::shared_ptr<LayerValue>&& value) override;
    void add(std::string&& value) override;
    void add(int&& value) override;

    void remove(unsigned int index) override;

    std::shared_ptr<LayerValue>& operator[](unsigned int index) override { return this->elements[index]; }

    const_array_iterator begin_element() const override { return this->elements.begin(); }
    const_array_iterator end_element() const override { return this->elements.end(); }

  private:
    std::vector<std::shared_ptr<LayerValue>> elements;
  };
}

#endif //GARLICCONFIG_LAYER_H
