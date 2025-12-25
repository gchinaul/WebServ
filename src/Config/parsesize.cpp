
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

size_t ConfigParser::parseSize(const std::string &s, size_t def)
{

    if (s.empty())
        return (def);

    std::string v = s;
    size_t mul = 1;

    char last = v[v.size() - 1];

    if (!std::isdigit((unsigned char)last))
    {
        if (last == 'K' || last == 'k')
            mul = 1024;
        else if (last == 'M' || last == 'm')
            mul = 1024 * 1024;
        else if (last == 'G' || last == 'g')
            mul = 1024 * 1024 * 1024;
        v = v.substr(0, v.size() - 1);
    }

    unsigned long val = Libft::strtoul(v);
    if (val == 0)
        return (def);

    return ((size_t)val * mul);
}
