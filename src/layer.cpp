//
// Created by Peyman Mortazavi on 9/21/18.
//

#include <boost/algorithm/string.hpp>

#include "layer.h"


using namespace std;

using namespace garlic;


// ObjectValue
shared_ptr<LayerValue> ObjectValue::clone() const {
  auto clone = make_shared<ObjectValue>();
  clone->property_list = this->property_list;
  return clone;
}

const shared_ptr<LayerValue>& ObjectValue::get(const string &key) const {
  auto finder = this->property_list.find(key);
  if (finder != this->property_list.end()) {
    return finder->second;
  }
  return NotFoundPtr;
}

const shared_ptr<LayerValue>& ObjectValue::resolve(const string &path) const {
  vector<string> parts;
  boost::split(parts, path, boost::is_any_of("."));
  const shared_ptr<LayerValue>* layer_value = &NotFoundPtr;
  const LayerValue* root = this;
  try {
    for (const auto &part : parts) {
      if (const auto &result = root->get(part)) {
        layer_value = &result;
        root = result.get();
      } else { return NotFoundPtr; }
    }
  } catch (const TypeError& error) { return NotFoundPtr; }
  return *layer_value;
}

void ObjectValue::apply(const shared_ptr<LayerValue> &layer) {
  for(const auto& member : layer->get_object()) {
    if (auto& existing_value = this->property_list[member.first]) {
      if (existing_value->is_object()) {
        // Copy the object and then apply the top layer.
        existing_value = existing_value->clone();
        existing_value->apply(member.second);
        continue;
      }
    }
    this->set(member.first, member.second);
  }
}


// ListValue
ListValue::ListValue(size_t size) {
  this->elements.reserve(size);
}

shared_ptr<LayerValue> ListValue::clone() const {
  auto clone = make_shared<ListValue>();
  clone->elements = this->elements;
  return clone;
}

void ListValue::add(const shared_ptr<LayerValue> &value) {
  this->elements.push_back(value);
}

void ListValue::add(shared_ptr<LayerValue> &&value) {
  this->elements.push_back(std::move(value));
}

void ListValue::add(string &&value) {
  this->elements.push_back(make_shared<StringValue>(move(value)));
}

void ListValue::add(int &&value) {
  this->elements.push_back(make_shared<IntegerValue>(value));
}

void ListValue::remove(unsigned int index) {
  this->elements.erase(this->elements.begin() + index);
}