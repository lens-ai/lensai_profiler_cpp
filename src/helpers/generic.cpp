#include <filesystem>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#include <cstring>
#include <cstdlib>

#include <datatracer_log.h>

static std::string removeTrailingSlash(const std::string& path) {
    if (!path.empty() && path.back() == '/') {
        return path.substr(0, path.size() - 1);
    }
    return path;
}

int acquire_lock(const std::string& file_path) {
    const std::string lock_file = removeTrailingSlash(file_path) + "_lock";
    int fd = open(lock_file.c_str(), O_CREAT | O_RDWR, 0666);

    if (fd == -1) {
        log_err << lock_file << " : " << fd << std::endl;
        return -1;
    }

    if (flock(fd, LOCK_EX) == -1) {
        log_err << "flock failed : " << lock_file << " : " << fd << std::endl;
        return -1;
    }
    log_debug << lock_file << " : " << fd << " success" << std::endl;
    return fd;
}

int release_lock(int fd) {
    if (fd == -1) return -1;

    if (flock(fd, LOCK_UN) == -1) {
        close(fd);
        log_err << fd << " unlock error" << std::endl;
        return -1;
    }
    close(fd);
    log_debug << fd << " success" << std::endl;
    return 0;
}

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

bool createFolder(const std::string& Savepath) {
    try {
        if (!std::filesystem::exists(Savepath)) {
            std::filesystem::create_directories(Savepath);
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error creating Savepath: " << e.what() << std::endl;
        return false;
    }
    return true;
}

