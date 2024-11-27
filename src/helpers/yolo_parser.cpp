#include <tuple>
#include <vector>
#include "yolo_parser.h"

std::vector<std::pair<float, int>> YOLOParser::parse(const void* raw_output) const {
    // Cast to the appropriate type
    const auto& detections = *static_cast<const std::vector<std::tuple<float, int, float, float>>*>(raw_output);

    std::vector<std::pair<float, int>> results;
    for (const auto& detection : detections) {
        // Use std::get with detection explicitly marked as const
        float score = std::get<0>(detection);  // Get the first element (float score)
        int class_id = std::get<1>(detection); // Get the second element (int class_id)

        results.emplace_back(score, class_id); // Add (score, class_id) to results
    }
    return results;
}
