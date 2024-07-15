#include <filesystem>

bool createFolderIfNotExists(const std::string& statSavepath, const std::string& dataSavepath) {
  try {
    if (!std::filesystem::exists(statSavepath)) {
      std::filesystem::create_directories(statSavepath);
      }
    if(!std::filesystem::exists(dataSavepath)) {
      std::filesystem::create_directories(dataSavepath);		    
      }
    } catch (const std::filesystem::filesystem_error& e) {
      // Handle other filesystem errors (optional)
      return false;
    }
  return true;
}
