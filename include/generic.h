#ifndef GENERIC_H
#define GENERIC_H

#include <string>  // For std::string

// Forward declaration for potential future class usage (optional)
// class FileSystemHelper;

namespace fs = std::filesystem;  // Namespace alias for readability

/**
 * @brief Checks if a path exists and creates a folder if it doesn't exist.
 *
 * This function attempts to create a directory at the specified path.
 * It returns true if the folder is created or already exists, and false
 * if an error occurs during the operation.
 *
 * @param path The path to the directory to create.
 *
 * @return bool True if the folder is created or already exists, false on error.
 */
bool createFolderIfNotExists(const std::string& statSavepath, const std::string& dataSavepath);

bool createFolder(const std::string& Savepath);
int acquire_lock(const std::string& file_path);
int release_lock(int fd);

#endif // GENERIC_H

