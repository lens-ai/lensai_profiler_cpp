#include "modeloutput_parser.h"
#include "resnet_parser.h"
#include <algorithm>

std::vector<std::pair<float, int>> ResNetParser::parse(const void* raw_output) const {
    const auto* resnet_output = static_cast<const std::vector<float>*>(raw_output);

    std::vector<std::pair<float, int>> results;
    for (size_t i = 0; i < resnet_output->size(); ++i) {
        results.emplace_back((*resnet_output)[i], static_cast<int>(i));
    }
    return results;
}

