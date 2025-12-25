
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>

#include "../include/Webserv.hpp"

std::string Libft::itoa(long n)
{

    std::ostringstream to_string;
    to_string << n;

    return (to_string.str());
}