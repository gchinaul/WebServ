
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>

#include "../include/Webserv.hpp"

std::string Libft::pathJoin(const std::string &a, const std::string &b)
{

    if (a.empty())
        return (b);

    std::string out = a;

    if (!out.empty() && out[out.size() - 1] != '/')
        out += '/';

    out += b;

    return (out);
}

std::string Libft::safePath(const std::string &root, const std::string &urlPath)
{

    std::string r = root;

    if (!r.empty() && r[r.size() - 1] != '/')
        r += '/';

    std::string p = urlPath;

    if (!p.empty() && p[0] == '/')
        p = p.substr(1);

    return (r + p);
}
