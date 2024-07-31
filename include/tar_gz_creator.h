#ifndef TAR_GZ_CREATOR_H
#define TAR_GZ_CREATOR_H

#include <libtar.h>
#include <vector>
#include <string>
#include <mutex>

class TarGzCreator {
public:
    TarGzCreator();
    bool createTar(const std::string& tarFilePath, const std::vector<std::string>& filePaths,const std::string& basePath);
    bool compressToGz(const std::string& tarFilePath, const std::string& gzFilePath);
    std::vector<std::string> collectFilesFromFolders(const std::vector<std::string>& folders);
    bool decompressGz(const std::string& gzFilePath, const std::string& outputFilePath);
    bool unpackTar(const std::string& tarFilePath, const std::string& outputFolderPath);
    bool emptyFolder(const std::string& folderPath); // New function to empty folder

private:
    void add_file_to_tar(TAR *tar, const std::string& path,
                            const std::string& basePath);
    std::mutex folderMutex; // Mutex for thread-safe folder access
};

#endif // TAR_GZ_CREATOR_H
