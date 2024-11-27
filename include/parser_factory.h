#ifndef PARSER_FACTORY_H
#define PARSER_FACTORY_H

#include <memory>
#include <string>
#include "modeloutput_parser.h" // Base interface for parsers

class ParserFactory {
public:
    /**
     * @brief Creates a parser object based on the model type.
     * 
     * @param model_type A string specifying the model type (e.g., "YOLO", "ResNet").
     * @return A unique pointer to the created parser object.
     * @throws std::invalid_argument if the model type is unsupported.
     */
    static std::unique_ptr<ModelOutputParser> createParser(const std::string& model_type);
};

#endif // PARSER_FACTORY_H
