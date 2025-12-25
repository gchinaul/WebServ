
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <cctype>
#include <vector>
#include <set>

#include "../include/LibftCpp.hpp"
#include "ConfigParser.hpp"
#include "Logger.hpp"
#include "Config.hpp"
#include "Message.hpp"

void ConfigParser::parseConfigFile()
{
    std::set<std::string> seenServerAddr;

    std::ifstream ifsFile(_filePath.c_str());
    if (!ifsFile.is_open())
    {

        Logger::error(ERROR_ON_OPEN_FILE + _filePath);
        return;
    }

    std::string line;
    while (std::getline(ifsFile, line))
    {

        if (line.empty() || line[0] == '#')
            continue;

        trimLeadingSpaces(line);

        if (line.find("server") == 0 && line.find('{') != std::string::npos)
        {

            parseServerBlock(ifsFile);
            
            std::string addr = _config.host + ":" + Libft::itoa(_config.port);
            std::string key = addr + "_" + _config.serverName;
            
            if (seenServerAddr.find(key) != seenServerAddr.end())
            {
                Logger::error("Duplicate server '" + _config.serverName + 
                             "' on " + addr + " - configuration rejected");
                _hasError = true;
                ifsFile.close();
                return;
            }
            seenServerAddr.insert(key);
            
            for (size_t i = 0; i < _additionalListens.size(); ++i)
            {
                std::string additionalAddr = _additionalListens[i].first + ":" + 
                                            Libft::itoa(_additionalListens[i].second);
                std::string additionalKey = additionalAddr + "_" + _config.serverName;
                
                if (seenServerAddr.find(additionalKey) != seenServerAddr.end())
                {
                    Logger::error("Duplicate server '" + _config.serverName + 
                                 "' on " + additionalAddr + " - configuration rejected");
                    _hasError = true;
                    ifsFile.close();
                    return;
                }
                seenServerAddr.insert(additionalKey);
            }
            
            continue;
        }

        if (line == "}")
            continue;

        parseLine(line, _configMap);
    }

    ifsFile.close();
}
