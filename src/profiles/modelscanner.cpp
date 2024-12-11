#include "ModelScanner.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <fstream>
#include <iostream>

// Generate expected data (metadata, operators) from a TensorFlow model and write to a .ini file
bool generateExpectedData(const tflite::Model* tfModel, const std::string& iniFilePath) {
    boost::property_tree::ptree iniData;

    // Extract metadata
    auto metadata = tfModel->metadata();
    if (metadata) {
        for (int i = 0; i < metadata->size(); i++) {
            auto meta = metadata->Get(i);
            iniData.add("Metadata.Keys", meta->name()->str());
            iniData.put("Metadata.Values." + meta->name()->str(), meta->description()->str());
        }
    }

    // Extract operators
    auto opCodes = tfModel->operator_codes();
    if (opCodes) {
        for (int i = 0; i < opCodes->size(); i++) {
            auto op = opCodes->Get(i);
            auto opName = tflite::EnumNameBuiltinOperator(tflite::GetBuiltinCode(op));
            iniData.add("Operators", opName);
        }
    }

    // Write to .ini file
    try {
        boost::property_tree::write_ini(iniFilePath, iniData);
    } catch (const std::exception& e) {
        std::cerr << "Failed to write to INI file: " << e.what() << std::endl;
        return false;
    }

    return true;
}

// Validate model metadata using the .ini file
bool validateMetadata(const tflite::Model* tfModel, const std::string& iniFilePath) {
    boost::property_tree::ptree iniData;
    try {
        boost::property_tree::read_ini(iniFilePath, iniData);
    } catch (const std::exception& e) {
        std::cerr << "Failed to read INI file: " << e.what() << std::endl;
        return false;
    }

    auto metadata = tfModel->metadata();
    if (metadata) {
        for (int i = 0; i < metadata->size(); i++) {
            auto meta = metadata->Get(i);
            std::string key = meta->name()->str();
            std::string value = meta->description()->str();

            // Validate keys
            if (iniData.get("Metadata.Keys", "").find(key) == std::string::npos) {
                std::cerr << "Unexpected metadata key: " << key << std::endl;
                return false;
            }

            // Validate values
            if (iniData.get("Metadata.Values." + key, "") != value) {
                std::cerr << "Metadata value mismatch for key: " << key << std::endl;
                return false;
            }
        }
    }

    return true;
}

// Validate operators against the expected ones from .ini file
bool validateOperators(const tflite::Model* tfModel, const std::string& iniFilePath) {
    boost::property_tree::ptree iniData;
    try {
        boost::property_tree::read_ini(iniFilePath, iniData);
    } catch (const std::exception& e) {
        std::cerr << "Failed to read INI file: " << e.what() << std::endl;
        return false;
    }

    std::set<std::string> expectedOps;
    for (auto& item : iniData.get_child("Operators")) {
        expectedOps.insert(item.second.data());
    }

    auto opCodes = tfModel->operator_codes();
    if (opCodes) {
        for (int i = 0; i < opCodes->size(); i++) {
            auto op = opCodes->Get(i);
            auto opName = tflite::EnumNameBuiltinOperator(tflite::GetBuiltinCode(op));
            if (expectedOps.find(opName) == expectedOps.end()) {
                std::cerr << "Unexpected operator: " << opName << std::endl;
                return false;
            }
        }
    }

    return true;
}

// Validate tensor buffers (check if tensor buffers are valid within model)
bool validateTensorBuffers(const tflite::Model* tfModel) {
    auto subgraphs = tfModel->subgraphs();
    for (int i = 0; i < subgraphs->size(); i++) {
        auto tensors = subgraphs->Get(i)->tensors();
        for (int j = 0; j < tensors->size(); j++) {
            auto tensor = tensors->Get(j);
            auto bufferIndex = tensor->buffer();
            if (bufferIndex < 0 || bufferIndex >= tfModel->buffers()->size()) {
                std::cerr << "Invalid buffer index detected for tensor: " << j << std::endl;
                return false;
            }
        }
    }
    return true;
}

// Validate operators for PyTorch models
bool validatePyTorchOperators(const torch::jit::script::Module& ptModel, const std::string& iniFilePath) {
    std::set<std::string> extractedOps;

    // Extract operators from PyTorch model
    for (const auto& method : ptModel.get_methods()) {
        for (const auto& instruction : method.function().instructions()) {
            extractedOps.insert(c10::toString(instruction.opname()));
        }
    }

    // Read expected ops from .ini file
    boost::property_tree::ptree iniData;
    try {
        boost::property_tree::read_ini(iniFilePath, iniData);
    } catch (const std::exception& e) {
        std::cerr << "Failed to read INI file: " << e.what() << std::endl;
        return false;
    }

    std::set<std::string> expectedOps;
    for (auto& item : iniData.get_child("Operators")) {
        expectedOps.insert(item.second.data());
    }

    // Validate operators
    for (const auto& op : extractedOps) {
        if (expectedOps.find(op) == expectedOps.end()) {
            std::cerr << "Unexpected PyTorch operator: " << op << std::endl;
            return false;
        }
    }

    return true;
}
