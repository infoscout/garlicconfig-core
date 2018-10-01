//
// Created by Peyman Mortazavi on 9/30/18.
//

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

#include "json.h"

using namespace std;

using namespace rapidjson;

using namespace garlic;


shared_ptr<LayerValue> map_value(const Value& value) {
  if (value.IsObject()) {
    auto object_value = make_shared<ObjectValue>();
    for (const auto& element : value.GetObject()) {
      object_value->set(element.name.GetString(), map_value(element.value));
    }
    return object_value;
  } else if (value.IsArray()) {
    auto array = value.GetArray();
    auto list_value = make_shared<ListValue>(array.Size());
    for (const auto& item : array) {
      list_value->add(map_value(item));
    }
    return list_value;
  } else if (value.IsString()) {
    return make_shared<StringValue>(value.GetString());
  } else if (value.IsInt()) {
    return make_shared<IntegerValue>(value.GetInt());
  } else if (value.IsDouble()) {
    return make_shared<DoubleValue>(value.GetDouble());
  } else if (value.IsBool()) {
    return make_shared<BoolValue>(value.GetBool());
  } else {
    throw TypeError();
  }
}


shared_ptr<LayerValue> JsonDecoder::load(std::istream &input_stream) const {
  // First, load up the JSON
  IStreamWrapper wrapper_input_stream(input_stream);
  Document document;
  document.ParseStream(wrapper_input_stream);
  assert(document.IsObject());
  return map_value(document);
}