#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <tuple>
#include <vector>

// Define common types used in parsers
using YOLOOutput = std::vector<std::tuple<float, int, float, float>>; // (score, class_id, x, y)
using ResNetOutput = std::vector<float>; // Probabilities for each class
using ModelOutput = std::vector<std::pair<float, int>>;  // Example definition: vector of score-class pairs

#endif // COMMON_TYPES_H
