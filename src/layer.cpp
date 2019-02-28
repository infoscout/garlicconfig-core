//
// Created by Peyman Mortazavi on 9/21/18.
//

#include <memory>

#include "layer.h"


using namespace std;

using namespace garlic;


struct string_partitioner {
  explicit string_partitioner(const string& text, char separator='.') : _text(text), _separator(separator) {}
  bool next(std::string& word) {
    for(auto index = _start; index < _text.length(); index++) {
      if (_text[index] == _separator) {
        if (_count) {
          word = _text.substr(_start, _count);
          _start = index;
          _count = 0;
          return true;
        } else {
          _start = index + 1;
        }
      } else {
        _count++;
      }
    }
    if (_count) {
      word = _text.substr(_start);  // update the word one last time if there has been any string left.
      _start = _text.length();  // this'll make the next run not enter the loop.
      _count = 0;  // this'll make the next run not come here again.
      return true;
    }
    return false;
  }
  template<typename T> static void for_each(const std::string& text, T func, char separator='.') {
    string_partitioner it{text, separator};
    std::string route;
    while (it.next(route)) func(route);
  }

private:
  const std::string& _text;
  char _separator;
  unsigned long _start = 0;
  unsigned long _count = 0;
};


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
  string_partitioner::for_each(path, [&parts](string item) {parts.emplace_back(move(item));});
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