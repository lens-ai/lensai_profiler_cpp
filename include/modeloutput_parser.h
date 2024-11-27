#ifndef MODELOUTPUT_PARSER_H
#define MODELOUTPUT_PARSER_H

#include <vector>
#include <utility>

class ModelOutputParser {
public:
    virtual ~ModelOutputParser() = default;
    virtual std::vector<std::pair<float, int>> parse(const void* raw_output) const = 0;
};

#endif // MODELOUTPUT_PARSER_H
