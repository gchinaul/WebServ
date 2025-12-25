
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

void ConfigParser::parseLocations()
{

    for (MAP_STR_STR::iterator it = _configMap.begin(); it != _configMap.end(); ++it)
    {

        if (it->first.find("location_") == 0)
        {

            std::string key = it->first;
            size_t pathStart = key.find("_") + 1;
            size_t pathEnd = key.rfind(".");

            if (pathEnd == std::string::npos || pathEnd <= pathStart)
                continue;

            std::string path = key.substr(pathStart, pathEnd - pathStart);
            std::string directive = key.substr(pathEnd + 1);
            RouteConf *route = findOrCreateRoute(path);

            if (directive == "methods")
            {

                route->allowedMethods.clear();
                std::istringstream iss(it->second);
                std::string method;
                while (iss >> method)
                {

                    for (size_t k = 0; k < method.size(); ++k)
                        method[k] = (char)Libft::toUpper((unsigned char)method[k]);
                    route->allowedMethods.insert(method);
                }
            }
            else if (directive == "root")
                route->root = it->second;
            else if (directive == "autoindex")
                route->autoindex = (it->second == "on");
            else if (directive == "index")
                route->index = it->second;
            else if (directive == "upload_store")
            {

                route->hasUpload = true;
                route->uploadStore = it->second;
            }
            else if (directive == "return")
            {

                std::istringstream issRet(it->second);
                std::string first, second;

                issRet >> first;
                if (first.empty())
                    Logger::error("ConfigParser: return directive empty for location " + path);
                else if (first.find_first_not_of("0123456789") == std::string::npos)
                {

                    route->redirectCode = Libft::atoi(first.c_str());
                    if (issRet >> second)
                    {

                        route->redirectUrl = second;
                        route->hasRedirect = true;
                    }
                    else
                    {

                        Logger::error("ConfigParser: return directive for location " + path + " has status code but no URL");
                        route->hasRedirect = false;
                    }
                }
                else
                {

                    route->redirectUrl = first;
                    route->redirectCode = 301;
                    route->hasRedirect = true;
                }
            }
            else if (directive == "cgi_ext")
            {

                route->hasCGI = true;
                std::istringstream issExt(it->second);
                std::string ext;
                while (issExt >> ext)
                {

                    if (!ext.empty() && ext[0] == ' ')
                        ext = ext.substr(1);
                    if (ext == ".py")
                        route->cgiMap[".py"] = "/usr/bin/python3";
                    else if (ext == ".c")
                        route->cgiMap[".c"] = "/usr/bin/gcc";
                    else if (ext == ".php")
                        route->cgiMap[".php"] = "/usr/bin/php-cgi";
                }
            }
            else if (directive == "cgi_bin")
            {

                std::istringstream issBin(it->second);
                std::string bin;
                while (issBin >> bin)
                    route->cgiBin = bin;
                route->hasCGI = true;
            }
        }
    }
}
