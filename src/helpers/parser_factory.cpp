#include "parser_factory.h"

// Implementation of the createParser method
std::unique_ptr<ModelOutputParser> ParserFactory::createParser(const std::string& model_type) {
    if (model_type == "YOLO") {
        return std::make_unique<YOLOParser>();
    } else if (model_type == "ResNet") {
        return std::make_unique<ResNetParser>();
    }
    throw std::invalid_argument("Unsupported model type: " + model_type);
}
