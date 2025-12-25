
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>

#include "../include/Webserv.hpp"

bool Libft::startsWith(const std::string &s, const std::string &p)
{

    if (p.size() > s.size())
        return (false);

    return (s.compare(0, p.size(), p) == 0);
}

bool Libft::endsWith(const std::string &s, const std::string &suf)
{

    if (suf.size() > s.size())
        return false;

    return (s.compare(s.size() - suf.size(), suf.size(), suf) == 0);
}
