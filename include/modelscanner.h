#ifndef MODEL_SCANNER_H
#define MODEL_SCANNER_H

#include <string>
#include <set>
#include <map>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <tensorflow/lite/model.h>
#include <torch/script.h>

// ModelScanner class declaration
class ModelScanner {
public:
    static bool generateExpectedData(const tflite::Model* tfModel, const std::string& iniFilePath);
    static bool validateMetadata(const tflite::Model* tfModel, const std::string& iniFilePath);
    static bool validateOperators(const tflite::Model* tfModel, const std::string& iniFilePath);
    static bool validateTensorBuffers(const tflite::Model* tfModel);
    static bool validatePyTorchOperators(const torch::jit::script::Module& ptModel, const std::string& iniFilePath);
    static bool scanModelOperators(const tflite::Model* model, const std::string& iniFilePath);
    static bool scanTensorBuffers(const tflite::Model* model);
    static bool analyzeMetadata(const tflite::Model* model);
};

#endif // MODEL_SCANNER_H
