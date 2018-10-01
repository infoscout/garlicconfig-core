//
// Created by Peyman Mortazavi on 9/30/18.
//

#ifndef GARLICCONFIG_ENCODING_H
#define GARLICCONFIG_ENCODING_H

#include <iostream>
#include <memory>

#include "layer.h"


namespace garlic {

  class Encoder {
  public:
    virtual void dump(std::ostream& output_stream, const LayerValue& value) const = 0;
  };


  class Decoder {
  public:
    virtual std::shared_ptr<LayerValue> load(std::istream& input_stream) const = 0;
  };

}

#endif //GARLICCONFIG_ENCODING_H
