
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <cctype>
#include <vector>

#include "../include/LibftCpp.hpp"
#include "ConfigParser.hpp"
#include "Logger.hpp"
#include "Config.hpp"
#include "Message.hpp"

ConfigParser::ConfigParser(const std::string &filePath) : _filePath(filePath), _hasError(false)
{

    parseConfigFile();
};

ConfigParser::~ConfigParser() {}

ConfigParser::ConfigParser() {}

ConfigParser::ConfigParser(const ConfigParser &obj)
{

    (void)obj;
}

ConfigParser &ConfigParser::operator=(const ConfigParser &obj)
{

    (void)obj;
    return (*this);
}

void ConfigParser::parseServerBlock(std::ifstream &file)
{

    MAP_STR_STR localMap;

    std::string line;
    while (std::getline(file, line))
    {

        trimLeadingSpaces(line);

        if (line.empty() || line[0] == '#')
            continue;
        if (line == "}")
            break;
        if (line.find("location") == 0 && line.find('{') != std::string::npos)
        {

            parseBlock(file, line, localMap);
            continue;
        }

        trimTrailingComment(line);

        parseLine(line, localMap);
    }

    MAP_STR_STR old = _configMap;
    _configMap = localMap;
    setConfig();
    _configMap = old;
}

ServerConf ConfigParser::getConfig() const { return (_config); }

std::string ConfigParser::getHost() const { return (_config.host); }
std::string ConfigParser::getServerName() const { return (_config.serverName); }
std::string ConfigParser::getErrorPages() const { return _config.errorDir; }

size_t ConfigParser::getClientMaxBodySize() const { return (_config.clientMaxBody); }

std::vector<ServerConf> ConfigParser::getConfigs() const { return _configs; }

bool ConfigParser::hasError() const { return _hasError; }
