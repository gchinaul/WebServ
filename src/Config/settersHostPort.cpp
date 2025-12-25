
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

void ConfigParser::setHostAndPorts(MAP_STR_STR::iterator it)
{

    if (it != _configMap.end())
    {

        std::string listen = it->second;
        std::istringstream iss(listen);
        std::string token;
        std::vector<std::pair<std::string, int> > listens;

        while (iss >> token)
        {

            size_t colonPos = token.find(':');
            std::string h;
            int p = 0;

            if (colonPos != std::string::npos)
            {

                h = token.substr(0, colonPos);
                p = Libft::atoi(token.substr(colonPos + 1).c_str());
            }
            else
            {

                h = "0.0.0.0";
                p = Libft::atoi(token.c_str());
            }
            listens.push_back(std::make_pair(h, p));
        }
        if (listens.empty())
        {

            _config.host = "localhost";
            _config.port = 8080;
        }
        else
        {

            _config.host = listens[0].first;
            _config.port = listens[0].second;
            _additionalListens = std::vector<std::pair<std::string, int> >(listens.begin() + 1, listens.end());
        }
    }
    else
    {

        _config.host = "localhost";
        _config.port = 8080;
    }
}
