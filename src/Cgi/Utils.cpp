#include <sys/resource.h>
#include <fcntl.h>
#include <fstream>
#include <limits.h>

#include "../include/Webserv.hpp"

bool CGI::isCGIRequest(const std::string &uri, const RouteConf &route)
{

    size_t dot = uri.find_last_of('.');
    if (dot == std::string::npos)
        return (false);

    std::string extension = uri.substr(dot);

    return (route.cgiMap.find(extension) != route.cgiMap.end());
}

std::string getCGIExecutable(const std::string &uri, const RouteConf &route)
{

    size_t dot = uri.find_first_of('.');
    std::string extension = uri.substr(dot);

    std::map<std::string, std::string>::const_iterator it = route.cgiMap.find(extension);
    if (it != route.cgiMap.end())
        return (it->second);

    return ("");
}
