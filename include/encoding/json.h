//
// Created by Peyman Mortazavi on 9/30/18.
//

#ifndef GARLICCONFIG_JSON_H
#define GARLICCONFIG_JSON_H

#include <memory>

#include "encoding.h"


namespace garlic {

  class JsonDecoder : public Decoder {
  public:
    std::shared_ptr<LayerValue> load(std::istream& input_stream) const;
  };

}

#endif //GARLICCONFIG_JSON_H
