#include <filesystem>
#include <iostream>

bool createFolderIfNotExists(const std::string& statSavepath, const std::string& dataSavepath) {
    try {
        if (!std::filesystem::exists(statSavepath)) {
            std::filesystem::create_directories(statSavepath);
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error creating statSavepath: " << e.what() << std::endl;
        return false;
    }

    try {
        if (!std::filesystem::exists(dataSavepath)) {
            std::filesystem::create_directories(dataSavepath);
        } 
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error creating dataSavepath: " << e.what() << std::endl;
        return false;
    }
    return true;
}
