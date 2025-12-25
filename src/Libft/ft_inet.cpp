
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>

#include "../include/Webserv.hpp"

in_addr_t Libft::ft_inet_addr(const char *host)
{
    if (!host || !*host)
        return INADDR_NONE;

    struct addrinfo hints;
    struct addrinfo *res = NULL;

    ft_memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICHOST;

    int err = getaddrinfo(host, NULL, &hints, &res);
    if (err == 0)
    {

        struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
        in_addr_t result = addr->sin_addr.s_addr;
        freeaddrinfo(res);
        return result;
    }

    ft_memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;

    err = getaddrinfo(host, NULL, &hints, &res);

    if (err != 0 || !res)
    {
        return (INADDR_NONE);
    }

    struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
    in_addr_t result = addr->sin_addr.s_addr;

    freeaddrinfo(res);

    return (result);
}

std::string Libft::ft_inet_ntoa(struct in_addr addr)
{
    unsigned long ip = ntohl(addr.s_addr);

    unsigned char bytes[4];
    bytes[0] = (ip >> 24) & 0xFF;
    bytes[1] = (ip >> 16) & 0xFF;
    bytes[2] = (ip >> 8) & 0xFF;
    bytes[3] = ip & 0xFF;

    char buffer[16];

    std::sprintf(buffer, "%u.%u.%u.%u",
                 bytes[0], bytes[1], bytes[2], bytes[3]);

    return (std::string(buffer));
}
