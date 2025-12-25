
#include "../include/Webserv.hpp"
#include "../include/ConfigParser.hpp"
#include "../include/Session.hpp"

bool Config::load(const std::string &path)
{

    if (parseConfig(path) == false)
        return (false);

    resetServers();
    configureSession(path);
    logConfig();

    return (true);
}

bool Config::parseConfig(const std::string &path)
{

    ConfigParser parser(path);

    if (parser.hasError())
    {
        Logger::error("Configuration parsing failed: " + path);
        return (false);
    }

    cfgs = parser.getConfigs();

    if (cfgs.empty())
    {

        Logger::error("No server blocks parsed in config: " + path);
        return (false);
    }
    return (true);
}
