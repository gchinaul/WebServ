
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>

#include "../include/Webserv.hpp"

char *Libft::xstrdup(const std::string &s)
{

    char *p = (char *)malloc(s.size() + 1);
    if (p)
    {

        for (size_t i = 0; i < s.size(); ++i)
            p[i] = s[i];
        p[s.size()] = '\0';
    }

    return (p);
}
