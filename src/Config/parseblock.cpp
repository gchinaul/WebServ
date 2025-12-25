
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

void ConfigParser::parseBlock(std::ifstream &file, const std::string &firstLine, MAP_STR_STR &targetMap)
{

    std::istringstream issFirstLine(firstLine);

    std::string token;
    issFirstLine >> token;

    std::string path;
    issFirstLine >> path;
    if (!path.empty() && path[0] == '"' && path[path.size() - 1] == '"' && path.size() >= 2)
        path = path.substr(1, path.size() - 2);

    if (path.empty())
    {

        Logger::error(ERROR_MISSING_PATH);
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {

        trimLeadingSpaces(line);

        if (line.empty() || line[0] == '#')
            continue;

        if (line == "}")
            break;

        trimTrailingComment(line);

        std::string key, value;
        std::istringstream issLine(line);
        issLine >> key;
        std::getline(issLine, value, ';');

        trimLeadingSpaces(value);
        value = normalizeValue(value);

        if (!key.empty())
        {

            std::string mapk = "location_" + path + "." + key;

            MAP_STR_STR::iterator mit = targetMap.find(mapk);
            if (mit != targetMap.end())
                mit->second += std::string(" ") + value;
            else
                targetMap[mapk] = value;
        }
    }
}
