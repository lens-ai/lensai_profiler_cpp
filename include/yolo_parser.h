#ifndef YOLO_PARSER_H
#define YOLO_PARSER_H

#include "modeloutput_parser.h" // Ensure this matches the base class header file.

class YOLOParser : public ModelOutputParser {
public:
    std::vector<std::pair<float, int>> parse(const void* raw_output) const override;
};

#endif // YOLO_PARSER_H
