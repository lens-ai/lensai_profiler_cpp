#ifndef MODELOUTPUT_PARSER_H
#define MODELOUTPUT_PARSER_H
#include <map>
#include <vector>
#include <string>
#include <type_traits>

// Base class for parsers
class ModelOutputParser {
public:
    virtual ~ModelOutputParser() = default;

    // Default parsing method for raw outputs (if needed)
    // virtual std::vector<std::pair<float, int>> parse(const void* raw_output) const = 0;

    // Template function to check if we need to pass-through or process the input dynamically
    template <typename T>
    auto processOutput(const T& output) const {
        if constexpr (std::is_same_v<T, std::map<std::string, std::vector<std::string>>>) {
            // If the input is already in the desired format, pass through
            return passThrough(output);
        } else {
            // Otherwise, process the output (e.g., parsing raw output)
            return parseRawOutput(output);
        }
    }

protected:
    // Pass-through method for already formatted output (no processing needed)
    std::map<std::string, std::vector<std::string>> passThrough(const std::map<std::string, std::vector<std::string>>& output) const {
        return output;  // Return as-is
    }

    // Example of processing raw output (could be for parsing, etc.)
    virtual std::vector<std::pair<float, int>> parseRawOutput(const void* raw_output) const = 0;
};
#endif // MODEL_OUTPUT_PARSER_H
