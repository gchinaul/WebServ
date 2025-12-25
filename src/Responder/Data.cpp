
#include <dirent.h>
#include <cstdlib>
#include <cstdio>
#include <sys/stat.h>
#include <unistd.h>

#include "Responder.hpp"
#include "Config.hpp"
#include "Server.hpp"
#include "MultipartParser.hpp"
#include "HTTP.hpp"
#include "LibftCpp.hpp"
#include "Session.hpp"
#include "Logger.hpp"

std::map<std::string, std::string> Responder::parseFormData(const std::string &body)
{

    std::map<std::string, std::string> result;

    std::vector<std::string> pairs = Libft::split(body, '&');

    for (size_t i = 0; i < pairs.size(); ++i)
    {
        size_t eq = pairs[i].find('=');

        if (eq != std::string::npos)
        {

            std::string key = Libft::urlDecode(pairs[i].substr(0, eq));
            std::string value = Libft::urlDecode(pairs[i].substr(eq + 1));
            result[key] = value;
        }
    }

    return (result);
}
