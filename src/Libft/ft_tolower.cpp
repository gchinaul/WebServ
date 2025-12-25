
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>

#include "../include/Webserv.hpp"

std::string Libft::toLower(const std::string &s)
{

    std::string result = s;

    for (size_t i = 0; i < result.size(); ++i)
    {

        if (result[i] >= 'A' && result[i] <= 'Z')
            result[i] = result[i] - 'A' + 'a';
    }

    return (result);
}
