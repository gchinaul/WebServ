
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>

#include "../include/Webserv.hpp"

int Libft::atoi(const std::string &s)
{

    std::istringstream iss(s);
    long v = 0;

    if (!(iss >> v))
    {
        return (0);
    }

    return (static_cast<int>(v));
}

int Libft::atoi(const char *s)
{

    if (!s)
        return (0);

    return (atoi(std::string(s)));
}
