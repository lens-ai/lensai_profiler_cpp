#ifndef TAR_GZ_CREATOR_H
#define TAR_GZ_CREATOR_H

#include <vector>
#include <string>
#include <boost/filesystem.hpp>
#include <libtar.h>  // Header for libtar
#include <zlib.h>

class TarGzCreator {
public:
    TarGzCreator();
    bool createTar(const std::string& tarFilePath, const std::vector<std::string>& filePaths);
    bool compressToGz(const std::string& tarFilePath, const std::string& gzFilePath);
    std::vector<std::string> collectFilesFromFolders(const std::vector<std::string>& folders);
    bool decompressGz(const std::string& gzFilePath, const std::string& outputFilePath);
    bool unpackTar(const std::string& tarFilePath, const std::string& outputFolderPath);

private:
    void addPadding(std::ofstream& stream, std::streamsize size);
};

#endif // TAR_GZ_CREATOR_H
