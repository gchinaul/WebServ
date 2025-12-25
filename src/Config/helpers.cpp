
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

std::string ConfigParser::normalizeValue(std::string &s)
{

    trimLeadingSpaces(s);
    trimTrailingSpaces(s);

    if (s.size() >= 2)
    {

        if ((s[0] == '"' && s[s.size() - 1] == '"') || (s[0] == '\'' && s[s.size() - 1] == '\''))
            return s.substr(1, s.size() - 2);
    }

    return (s);
}

void ConfigParser::trimLeadingSpaces(std::string &str)
{

    size_t first = str.find_first_not_of(" \t");
    if (first != std::string::npos)
        str = str.substr(first);
    else
        str.clear();
}

void ConfigParser::trimTrailingSpaces(std::string &str)
{

    size_t end = str.find_last_not_of(" \t\r\n");
    if (end != std::string::npos)
        str = str.substr(0, end + 1);
}

void ConfigParser::trimTrailingComment(std::string &str)
{

    size_t comment = str.find('#');
    if (comment != std::string::npos)
        str = str.substr(0, comment);
}
