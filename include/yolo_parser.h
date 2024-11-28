#ifndef YOLO_PARSER_H
#define YOLO_PARSER_H

#include "modeloutput_parser.h"
#include <vector>
#include <tuple>

class YOLOParser : public ModelOutputParser {
public:
    // Override the raw output parsing method for YOLO (just an example)
    std::vector<std::pair<float, int>> parseRawOutput(const void* raw_output) const override {
        // Assuming the raw output is of type std::vector<std::tuple<float, int, float, float>>
        auto detections = *reinterpret_cast<const std::vector<std::tuple<float, int, float, float>>*>(raw_output);
        std::vector<std::pair<float, int>> results;

        for (const auto& detection : detections) {
            float score = std::get<0>(detection);
            int class_id = std::get<1>(detection);
            results.push_back({score, class_id});
        }

        return results;
    }
};

#endif // YOLO_PARSER_H
