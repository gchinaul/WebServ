
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>

#include "../include/Webserv.hpp"

std::string Libft::join(const std::vector<std::string> &parts, const std::string &sep)
{

    std::string output;

    for (size_t i = 0; i < parts.size(); i++)
    {

        if (i)
            output += sep;
        output += parts[i];
    }

    return (output);
}