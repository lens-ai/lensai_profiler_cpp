#ifndef PARSER_FACTORY_H
#define PARSER_FACTORY_H

#include "modeloutput_parser.h"
#include "yolo_parser.h"
#include "resnet_parser.h"
#include <memory>
#include <stdexcept>

class ParserFactory {
public:
    // Factory method to create the appropriate parser based on model type
    static std::unique_ptr<ModelOutputParser> createParser(const std::string& model_type);
};

#endif // PARSER_FACTORY_H
