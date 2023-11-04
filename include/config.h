#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <map>

class Config {
public:
    static std::string get(std::string &&, std::string &&);

    static void load(const std::string &);

private:
    static std::map<std::string, std::map<std::string, std::string>> config;
};

std::map<std::string, std::map<std::string, std::string>> Config::config;

std::string Config::get(std::string &&section, std::string &&parameter) {
    if (config.count(section) && config[section].count(parameter)) {
        return config[section][parameter];
    }
    std::cerr << "Couldn't get parameter " << parameter << " from section " << section << std::endl;
    return {};
}


void Config::load(const std::string &config_path) {
    std::ifstream config_file(config_path);
    if (!config_file.is_open()) {
        std::cerr << "Failed to open the configuration file." << std::endl;
        return;
    }
    std::string currentSection;
    std::string line;
    while (std::getline(config_file, line)) {
        line.erase(line.find_last_not_of('\r') + 1);
        line.erase(line.find_last_not_of('\n') + 1);
        if (line.empty() || line[0] == ';') {
            continue;
        } else if (line[0] == '[' && line[line.length() - 1] == ']') {
            currentSection = line.substr(1, line.length() - 2);
        } else {
            size_t equalsPos = line.find('=');
            if (equalsPos != std::string::npos) {
                std::string key = line.substr(0, equalsPos);
                std::string value = line.substr(equalsPos + 1);
                config[currentSection][key] = value;
            }
        }
    }

    config_file.close();
}

