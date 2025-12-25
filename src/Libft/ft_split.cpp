
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>

#include "../include/Webserv.hpp"

std::vector<std::string> Libft::split(const std::string &str, char delim)
{

    std::vector<std::string> result;
    std::string current;

    for (size_t i = 0; i < str.size(); ++i)
    {

        if (str[i] == delim)
        {

            if (!current.empty())
            {

                result.push_back(current);
                current.clear();
            }
        }
        else
            current += str[i];
    }
    if (!current.empty())
        result.push_back(current);

    return (result);
}
