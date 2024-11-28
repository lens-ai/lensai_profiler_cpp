#ifndef RESNET_PARSER_H
#define RESNET_PARSER_H

#include "modeloutput_parser.h"
#include <vector>

class ResNetParser : public ModelOutputParser {
public:
    // Override the raw output parsing method for ResNet
    std::vector<std::pair<float, int>> parseRawOutput(const void* raw_output) const override {
        // Assuming raw_output is a std::vector<float>
        auto raw_data = *reinterpret_cast<const std::vector<float>*>(raw_output);
        std::vector<std::pair<float, int>> results;

        for (size_t i = 0; i < raw_data.size(); ++i) {
            results.push_back({raw_data[i], static_cast<int>(i)});
        }

        return results;
    }
};

#endif // RESNET_PARSER_H
