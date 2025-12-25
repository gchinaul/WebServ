
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

void ConfigParser::parseLine(std::string &line, MAP_STR_STR &targetMap)
{

    std::istringstream issLine(line);
    std::string key, value;

    issLine >> key;
    std::getline(issLine, value, ';');

    trimLeadingSpaces(value);
    value = normalizeValue(value);

    if (!key.empty() && !value.empty())
    {
        if (key == "listen" && targetMap.find("listen") != targetMap.end())
            targetMap[key] += " " + value;
        else
            targetMap[key] = value;
    }
    else if (!key.empty() && value.empty())
        Logger::error("Missing value for key: " + key);
}
