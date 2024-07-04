#include "iniparser.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

std::map<std::string, std::string> IniParser::parseIniFile(const std::string& filename, const std::string& section, const std::string& key) {
        boost::property_tree::ptree pt;
        try {
            // Read the INI file into the property tree
            boost::property_tree::ini_parser::read_ini(filename, pt);
        } catch (const std::exception& e) {
            std::cerr << "Error reading INI file: " << e.what() << std::endl;
            return {};
        }

        std::map<std::string, std::string> result;

        // Fetch all key-value pairs from the specified section
        try {
            if (key.empty()) {
                // Retrieve all keys if key is not specified
                for (const auto& item : pt.get_child(section)) {
                    result[item.first] = item.second.get_value<std::string>();
                }
            } else {
                // Retrieve a specific key if provided
                result[key] = pt.get<std::string>(section + "." + key);
            }
        } catch (const std::exception& e) {
            std::cerr << "Error fetching from section: " << section << ". Error: " << e.what() << std::endl;
            result.clear();
        }

        return result;
    }

    std::map<std::string, std::vector<std::string>> IniParser::parseIniFileNew(const std::string& filename, const std::string& section, const std::string& key) {
        boost::property_tree::ptree pt;
        try {
            // Read the INI file into the property tree
            boost::property_tree::ini_parser::read_ini(filename, pt);
        } catch (const std::exception& e) {
            std::cerr << "Error reading INI file: " << e.what() << std::endl;
            return {};
        }

        std::map<std::string, std::vector<std::string>> result;

        // Helper function to split a string by commas
        auto split = [](const std::string& s) {
            std::vector<std::string> elems;
            std::stringstream ss(s);
            std::string item;
            while (std::getline(ss, item, ',')) {
                elems.push_back(item);
		std::cout << item << std::endl;
            }
            return elems;
        };

        // Fetch global key-value pairs if no section is specified
        if (section.empty()) {
            try {
                for (const auto& item : pt) {
                    if (item.second.empty()) { // Global key-value pair
                        result[item.first] = split(item.second.data());
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "Error fetching global key-value pairs. Error: " << e.what() << std::endl;
                result.clear();
            }
        } else {
            // Fetch key-value pairs from the specified section
            try {
                if (key.empty()) {
                    // Retrieve all keys if key is not specified
                    for (const auto& item : pt.get_child(section)) {
                        result[item.first] = split(item.second.get_value<std::string>());
                    }
                } else {
                    // Retrieve a specific key if provided
                    result[key] = split(pt.get<std::string>(section + "." + key));
                }
            } catch (const std::exception& e) {
                std::cerr << "Error fetching from section: " << section << ". Error: " << e.what() << std::endl;
                result.clear();
            }
        }

        return result;
    }
