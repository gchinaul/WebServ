
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

void ConfigParser::setConfig()
{

    MAP_STR_STR::iterator it;

    for (MAP_STR_STR::iterator mit = _configMap.begin(); mit != _configMap.end(); ++mit)
    {

        if (mit->first.find("location_") == 0)
            Logger::debug("ConfigParser: parsed '" + mit->first + "' = '" + mit->second + "'");
    }

    it = _configMap.find("listen");

    setHostAndPorts(it);

    it = _configMap.find("server_name");
    if (it != _configMap.end())
        _config.serverName = it->second;
    else
        _config.serverName = "webserv";

    it = _configMap.find("root");
    if (it != _configMap.end())
        _config.root = it->second;
    else
        _config.root = "./www";

    it = _configMap.find("error_pages");
    if (it != _configMap.end())
        _config.errorDir = it->second;
    else
        _config.errorDir = "./errors";

    it = _configMap.find("client_max_body_size");
    if (it != _configMap.end())
        _config.clientMaxBody = parseSize(it->second, 1 * 1024 * 1024);
    else
        _config.clientMaxBody = 1024 * 1024;

    it = _configMap.find("cgi_timeout");
    if (it != _configMap.end())
        _config.cgiTimeout = Libft::atoi(it->second.c_str());
    else
        _config.cgiTimeout = 10;

    it = _configMap.find("cgi_max_input_bytes");
    if (it != _configMap.end())
        _config.cgiMaxInput = parseSize(it->second, 10 * 1024 * 1024);
    else
        _config.cgiMaxInput = _config.clientMaxBody;

    it = _configMap.find("index");
    if (it != _configMap.end())
        _config.defaultFile = it->second;
    else
        _config.defaultFile = "index.html";

    it = _configMap.find("session_ttl");
    if (it != _configMap.end())
        _config.sessionTTL = (unsigned int)Libft::atoi(it->second.c_str());
    else
        _config.sessionTTL = 3600;

    it = _configMap.find("cookie_secure");
    if (it != _configMap.end())
        _config.cookieSecure = (it->second == "on" || it->second == "true");
    else
        _config.cookieSecure = false;

    it = _configMap.find("cookie_samesite");
    if (it != _configMap.end())
        _config.cookieSameSite = it->second;
    else
        _config.cookieSameSite = "Lax";
    it = _configMap.find("session_persist");

    if (it != _configMap.end())
        _config.sessionPersist = it->second;
    else
        _config.sessionPersist = "";

    parseLocations();

    _configs.push_back(_config);

    for (size_t i = 0; i < _additionalListens.size(); ++i)
    {
        ServerConf dup = _config;
        dup.host = _additionalListens[i].first;
        dup.port = _additionalListens[i].second;
        _configs.push_back(dup);
    }
    _additionalListens.clear();
}
