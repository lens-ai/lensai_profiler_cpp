#include <filesystem>

bool createFolderIfNotExists(const std::string& path) {
  try {
    if (!std::filesystem::exists(path)) {
      std::filesystem::create_directory(path);
    }
  } catch (const std::filesystem::filesystem_error& e) {
    // Handle other filesystem errors (optional)
    return false;
  }
  return true;
}
