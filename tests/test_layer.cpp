//
// Created by Peyman Mortazavi on 9/28/18.
//

#include <iostream>
#include <memory>
#include <numeric>
#include <vector>

#include <gtest/gtest.h>

#include "garlicconfig.h"


using namespace std;

using namespace garlic;


int sum(const shared_ptr<LayerValue>& layer) {
  auto total = 0;
  for (const auto& item : layer->get_array()) {
    total += item->get_int();
  }
  return total;
}


TEST(LayerTests, NullValueTests) {
  auto value = NullValue();
  auto ptr = value.clone();
  ASSERT_TRUE(value.is_null());
  ASSERT_TRUE(ptr->is_null());
}


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


TEST(LayerTests, ResolveTests) {
  auto root_value = make_shared<ObjectValue>();

  // Go 5 levels deep.
  string level_key_name = "level0";
  root_value->set(level_key_name, make_shared<ObjectValue>());
  for(auto index = 1; index < 5; index++) {
    auto new_key_name = "level" + to_string(index);
    root_value->resolve(level_key_name)->set(new_key_name, make_shared<ObjectValue>());
    level_key_name += "." + new_key_name;
  }

  // non existing object should yield null pointer.
  ASSERT_EQ(root_value->resolve("non_existing_field"), NotFoundPtr);
  if (auto non_existing_value = root_value->resolve("a.b.c")) {  // if statement should be able to handle this.
    FAIL() << "a.b.c Doesn't exist so the if statement should not be entered.";
  }

  if (const auto& existing_value = root_value->resolve("level0.level1")) {
    existing_value->set("name", "Level 1");  // should be found and the pointer should be making changes on the original object.
  } else {
    FAIL() << "'level0.level1' was not found but it should have been.";
  }
  if (const auto& existing_value = root_value->resolve("level0.level1.name")) {
    ASSERT_EQ(existing_value->get_string(), "Level 1");
  } else {
    FAIL() << "'level0.level1.name' was not found but it should have been.";
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

  // Using size initializer
  auto pre_allocated_list_value = make_shared<ListValue>(3);
  pre_allocated_list_value->add(1);
  pre_allocated_list_value->add(2);
  pre_allocated_list_value->add(3);
  ASSERT_EQ(sum(pre_allocated_list_value), 6);
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


TEST(LayerTests, MergeTests) {
  // Load the layers from the data
  auto base_layer_str = "{\n"
                    "  \"name\": \"Base Layer\",\n"
                    "  \"base_raw_value\": 0,\n"
                    "  \"common_raw_value\": 0,\n"
                    "  \"base_object\": {\n"
                    "    \"base_value\": \"Should Not Be Touched\"\n"
                    "  },\n"
                    "  \"should_be_merged_object\": {\n"
                    "    \"base_raw_value_1\": 0,\n"
                    "    \"common_raw_value\": \"From Base\"\n"
                    "  },\n"
                    "  \"items\": [1, 2, 3]\n"
                    "}";
  auto layer1_str = "{\n"
                "  \"name\": \"Layer 1\",\n"
                "  \"common_raw_value\": 1,\n"
                "  \"layer1_object\": {\n"
                "    \"layer1_value\": \"Should Not Be Touched 1\"\n"
                "  },\n"
                "  \"should_be_merged_object\": {\n"
                "    \"layer1_raw_value_1\": 1,\n"
                "    \"common_raw_value\": \"From Layer 1\"\n"
                "  },\n"
                "  \"items\": [10, 20, 30]\n"
                "}";
  auto layer2_str = "{\n"
                "  \"name\": \"Layer 2\",\n"
                "  \"common_raw_value\": 2,\n"
                "  \"layer2_object\": {\n"
                "    \"layer2_value\": \"Should Not Be Touched 2\"\n"
                "  },\n"
                "  \"should_be_merged_object\": {\n"
                "    \"layer2_raw_value_1\": 2,\n"
                "    \"common_raw_value\": \"From Layer 2\"\n"
                "  },\n"
                "  \"items\": [100, 200, 300]\n"
                "}";
  auto decoder = JsonDecoder();
  auto repo = make_shared<MemoryConfigRepository>();
  repo->save("base_layer", [&base_layer_str](ostream& output_stream) { output_stream << base_layer_str; });
  repo->save("layer1", [&layer1_str](ostream& output_stream) { output_stream << layer1_str; });
  repo->save("layer2", [&layer2_str](ostream& output_stream) { output_stream << layer2_str; });

  auto base_layer = decoder.load(*repo->retrieve("base_layer"));
  auto layer1 = decoder.load(*repo->retrieve("layer1"));
  auto layer2 = decoder.load(*repo->retrieve("layer2"));
  auto expected_items = vector<int>{1, 2, 3};
  auto expected_total = accumulate(expected_items.begin(), expected_items.end(), 0);

  auto target = base_layer->clone();
  ASSERT_EQ(target->resolve("name")->get_string(), "Base Layer");
  ASSERT_EQ(target->resolve("base_raw_value")->get_int(), 0);
  ASSERT_EQ(target->resolve("common_raw_value")->get_int(), 0);
  ASSERT_EQ(target->resolve("base_object.base_value")->get_string(), "Should Not Be Touched");
  ASSERT_EQ(sum(target->resolve("items")), expected_total);
  ASSERT_EQ(target->resolve("layer1_object.layer1_value"), nullptr);
  ASSERT_EQ(target->resolve("layer2_object.layer2_value"), nullptr);
  ASSERT_EQ(target->resolve("should_be_merged_object.base_raw_value_1")->get_int(), 0);
  ASSERT_EQ(target->resolve("should_be_merged_object.common_raw_value")->get_string(), "From Base");
  ASSERT_EQ(target->resolve("should_be_merged_object.layer1_raw_value_1"), nullptr);
  ASSERT_EQ(target->resolve("should_be_merged_object.layer2_raw_value_1"), nullptr);

  target->apply(layer1);
  ASSERT_EQ(target->resolve("name")->get_string(), "Layer 1");
  ASSERT_EQ(target->resolve("base_raw_value")->get_int(), 0);
  ASSERT_EQ(target->resolve("common_raw_value")->get_int(), 1);
  ASSERT_EQ(target->resolve("base_object.base_value")->get_string(), "Should Not Be Touched");
  ASSERT_EQ(sum(target->resolve("items")), expected_total * 10);
  ASSERT_EQ(target->resolve("layer1_object.layer1_value")->get_string(), "Should Not Be Touched 1");
  ASSERT_EQ(target->resolve("layer2_object.layer2_value"), nullptr);
  ASSERT_EQ(target->resolve("should_be_merged_object.base_raw_value_1")->get_int(), 0);
  ASSERT_EQ(target->resolve("should_be_merged_object.common_raw_value")->get_string(), "From Layer 1");
  ASSERT_EQ(target->resolve("should_be_merged_object.layer1_raw_value_1")->get_int(), 1);
  ASSERT_EQ(target->resolve("should_be_merged_object.layer2_raw_value_1"), nullptr);

  target->apply(layer2);
  ASSERT_EQ(target->resolve("name")->get_string(), "Layer 2");
  ASSERT_EQ(target->resolve("base_raw_value")->get_int(), 0);
  ASSERT_EQ(target->resolve("common_raw_value")->get_int(), 2);
  ASSERT_EQ(target->resolve("base_object.base_value")->get_string(), "Should Not Be Touched");
  ASSERT_EQ(sum(target->resolve("items")), expected_total * 100);
  ASSERT_EQ(target->resolve("layer1_object.layer1_value")->get_string(), "Should Not Be Touched 1");
  ASSERT_EQ(target->resolve("layer2_object.layer2_value")->get_string(), "Should Not Be Touched 2");
  ASSERT_EQ(target->resolve("should_be_merged_object.base_raw_value_1")->get_int(), 0);
  ASSERT_EQ(target->resolve("should_be_merged_object.common_raw_value")->get_string(), "From Layer 2");
  ASSERT_EQ(target->resolve("should_be_merged_object.layer1_raw_value_1")->get_int(), 1);
  ASSERT_EQ(target->resolve("should_be_merged_object.layer2_raw_value_1")->get_int(), 2);

  // To make sure the apply function still uses the pointers to make sure extra objects aren't being created.
  ASSERT_EQ(target->resolve("base_raw_value").get(), base_layer->resolve("base_raw_value").get());
}
