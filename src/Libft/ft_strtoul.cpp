
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>

#include "../include/Webserv.hpp"

unsigned long Libft::strtoul(const std::string &s)
{

    std::istringstream iss(s);
    unsigned long v = 0;

    if (!(iss >> v))
        return (0);

    return (v);
}
