//
// Created by Peyman Mortazavi on 9/28/18.
//

#include <iostream>
#include <memory>
#include <vector>

#include <gtest/gtest.h>

#include "garlicconfig.h"


using namespace std;

using namespace garlic;


TEST(LayerTests, StringValueTests) {
  auto some_string_value = "Some String Value";
  auto string_value = StringValue(some_string_value);  // lvalue support.

  // Basic Type Checks
  ASSERT_TRUE(string_value.is_string());
  ASSERT_FALSE(string_value.is_bool());
  ASSERT_FALSE(string_value.is_object());
  ASSERT_FALSE(string_value.is_int());
  ASSERT_FALSE(string_value.is_double());
  ASSERT_FALSE(string_value.is_array());

  // Basic Get Checks
  ASSERT_EQ(string_value.get_string(), some_string_value);
  ASSERT_THROW(string_value.get_int(), TypeError);
  ASSERT_THROW(string_value.get_double(), TypeError);
  ASSERT_THROW(string_value.get_bool(), TypeError);
  ASSERT_THROW(string_value.get("any key"), TypeError);
  ASSERT_THROW(string_value.set("any key", "any value"), TypeError);

  // rvalue support.
  auto rvalue_string = "hi";
  string_value = StringValue(move(rvalue_string));  // rvalue test.
  ASSERT_EQ(string_value.get_string(), "hi");
}


TEST(LayerTests, IntegerValueTests) {
  auto some_int_value = 52;
  auto int_value = IntegerValue(some_int_value);  // lvalue support.

  // Basic Type Checks
  ASSERT_FALSE(int_value.is_string());
  ASSERT_FALSE(int_value.is_bool());
  ASSERT_FALSE(int_value.is_object());
  ASSERT_TRUE(int_value.is_int());
  ASSERT_FALSE(int_value.is_double());
  ASSERT_FALSE(int_value.is_array());

  // Basic Get Checks
  ASSERT_EQ(int_value.get_int(), some_int_value);
  ASSERT_THROW(int_value.get_string(), TypeError);
  ASSERT_THROW(int_value.get_double(), TypeError);
  ASSERT_THROW(int_value.get_bool(), TypeError);
  ASSERT_THROW(int_value.get("any key"), TypeError);
  ASSERT_THROW(int_value.set("any key", "any value"), TypeError);

  // rvalue support.
  auto rvalue_int = 12;
  int_value = IntegerValue(move(rvalue_int));  // rvalue test.
  ASSERT_EQ(int_value.get_int(), 12);
}


TEST(LayerTests, ObjectValueTests) {
  auto object_value = ObjectValue();

  // Basic Type Checks
  ASSERT_FALSE(object_value.is_string());
  ASSERT_FALSE(object_value.is_bool());
  ASSERT_TRUE(object_value.is_object());
  ASSERT_FALSE(object_value.is_int());
  ASSERT_FALSE(object_value.is_double());
  ASSERT_FALSE(object_value.is_array());

  // Basic Get Checks
  ASSERT_THROW(object_value.get_int(), TypeError);
  ASSERT_THROW(object_value.get_string(), TypeError);
  ASSERT_THROW(object_value.get_double(), TypeError);
  ASSERT_THROW(object_value.get_bool(), TypeError);

  // Get methods.
  ASSERT_EQ(object_value.get("name"), nullptr);
  object_value.set("name", "Peyman");  // passing LayerValue
  object_value.set("age", make_shared<IntegerValue>(25));  // passing rvalue shared_ptr

  auto school_object = make_shared<ObjectValue>();
  school_object->set("name", "University of Colorado Boulder");
  school_object->set("degree", "B.S.");

  object_value.set("school_info", school_object);  // passing lvalue shared_ptr
  ASSERT_EQ(school_object.use_count(), 2);

  ASSERT_EQ(object_value.get("name")->get_string(), "Peyman");
  ASSERT_EQ(object_value["age"]->get_int(), 25);
  ASSERT_EQ(object_value["non_existing_key"], nullptr);
  ASSERT_EQ(object_value["school_info"]->get("name")->get_string(), "University of Colorado Boulder");
  ASSERT_EQ(object_value["school_info"]->get("degree")->get_string(), "B.S.");

  vector<string> keys;
  for(auto it = object_value.begin_member(); it != object_value.end_member(); ++it) {
    keys.push_back(it->first);
  }
  ASSERT_EQ(keys.size(), 3);
  ASSERT_EQ(keys[0], "age");
  ASSERT_EQ(keys[1], "name");
  ASSERT_EQ(keys[2], "school_info");

  auto index = 0;
  for(const auto& item : object_value.get_object()) {
    ASSERT_EQ(keys[index], item.first);
    index++;
  }
}


TEST(LayerTests, ListValueTests) {
  auto list_value = ListValue();
  list_value.add(25);
  list_value.add("Peyman");

  ASSERT_EQ(list_value[0]->get_int(), 25);
  ASSERT_EQ(list_value[1]->get_string(), "Peyman");

  ASSERT_TRUE(list_value.is_array());

  list_value.remove(1);

  for(auto it = list_value.begin_element(); it != list_value.end_element(); ++it) {
    ASSERT_EQ((*it)->get_int(), 25);
  }

  for(const auto& item : list_value.get_array()) {
    ASSERT_EQ(item->get_int(), 25);
  }
}


TEST(LayerTests, BoolValueTests) {
  auto bool_value = BoolValue(false);

  // Basic Type Checks
  ASSERT_FALSE(bool_value.is_string());
  ASSERT_TRUE(bool_value.is_bool());
  ASSERT_FALSE(bool_value.is_object());
  ASSERT_FALSE(bool_value.is_int());
  ASSERT_FALSE(bool_value.is_double());
  ASSERT_FALSE(bool_value.is_array());

  ASSERT_EQ(bool_value.get_bool(), false);
}


TEST(LayerTests, DoubleValueTests) {
  auto double_value = DoubleValue(10.12);

  // Basic Type Checks
  ASSERT_FALSE(double_value.is_string());
  ASSERT_FALSE(double_value.is_bool());
  ASSERT_FALSE(double_value.is_object());
  ASSERT_FALSE(double_value.is_int());
  ASSERT_TRUE(double_value.is_double());
  ASSERT_FALSE(double_value.is_array());

  ASSERT_EQ(double_value.get_double(), 10.12);
}