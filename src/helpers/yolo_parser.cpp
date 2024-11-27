#include "yolo_parser.h"
#include <vector>

std::vector<std::pair<float, int>> YOLOParser::parse(const void* raw_output) const{
    const auto* yolo_output = static_cast<const std::vector<std::tuple<float, int, float, float>>*>(raw_output);

    std::vector<std::pair<float, int>> results;
    for (const auto& detection : *yolo_output) {
        float score = std::get<0>(detection);
        int class_id = std::get<1>(detection);
        results.emplace_back(score, class_id);
    }
    return results;
}
