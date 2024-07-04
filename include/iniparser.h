#ifndef INIPARSER_H
#define INIPARSER_H

#include <iostream>
#include <fstream>
#include <string>
#include <map>

/**
 * @brief Class for parsing INI files and extracting key-value pairs.
 */
class IniParser {
public:
    /**
     * @brief Parses the specified INI file and returns key-value pairs for the given section and subsection.
     * 
     * @param filename The path to the INI file.
     * @param section The name of the section in the INI file.
     * @param subsection The name of the subsection in the INI file.
     * @return A map containing the key-value pairs.
     */
    static std::map<std::string, std::string> parseIniFile(const std::string& filename, const std::string& section, const std::string& key);
    std::map<std::string, std::vector<std::string>> parseIniFileNew(const std::string& filename, const std::string& section, const std::string& key);
};

#endif // INIPARSER_H
