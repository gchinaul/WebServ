
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

RouteConf *ConfigParser::findOrCreateRoute(const std::string &path)
{

    for (size_t i = 0; i < _config.routes.size(); ++i)
    {

        if (_config.routes[i].path == path)
        {
            return (&_config.routes[i]);
        }
    }
    RouteConf newRoute;
    newRoute.path = path;
    newRoute.autoindex = false;
    newRoute.hasCGI = false;
    newRoute.hasRedirect = false;
    newRoute.hasUpload = false;
    newRoute.redirectCode = 301;
    newRoute.allowedMethods.insert("GET");
    _config.routes.push_back(newRoute);
    return &_config.routes.back();
}
