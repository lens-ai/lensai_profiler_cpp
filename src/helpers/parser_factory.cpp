#include "parser_factory.h"
#include "yolo_parser.h"
#include "resnet_parser.h"
#include <stdexcept>

std::unique_ptr<ModelOutputParser> ParserFactory::createParser(const std::string& model_type) {
    if (model_type == "YOLO") {
        return std::make_unique<YOLOParser>();
    } else if (model_type == "ResNet") {
        return std::make_unique<ResNetParser>();
    }
    throw std::invalid_argument("Unsupported model type: " + model_type);
}
