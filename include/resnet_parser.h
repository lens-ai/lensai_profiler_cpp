#ifndef RESNET_PARSER_H
#define RESNET_PARSER_H

#include "modeloutput_parser.h"

/**
 * @brief ResNet model output parser.
 */
class ResNetParser : public ModelOutputParser {
public:
    std::vector<std::pair<float, int>> parse(const void* raw_output) const override;
};

#endif // RESNET_PARSER_H
