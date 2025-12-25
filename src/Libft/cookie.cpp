
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>

#include "../include/Webserv.hpp"

std::map<std::string, std::string> Libft::parseCookies(const std::string &cookieHeader)
{

    std::map<std::string, std::string> out;

    size_t i = 0;
    while (i < cookieHeader.size())
    {

        while (i < cookieHeader.size() && (cookieHeader[i] == ' ' || cookieHeader[i] == '\t' || cookieHeader[i] == ';'))
            ++i;

        size_t eq = cookieHeader.find('=', i);

        if (eq == std::string::npos)
            break;

        std::string k = Libft::trim(cookieHeader.substr(i, eq - i));
        i = eq + 1;

        size_t semi = cookieHeader.find(';', i);

        std::string v = (semi == std::string::npos) ? cookieHeader.substr(i) : cookieHeader.substr(i, semi - i);

        out[k] = Libft::trim(v);
        if (semi == std::string::npos)
            break;

        i = semi + 1;
    }

    return (out);
}

std::string Libft::makeSetCookie(const std::string &name, const std::string &value, const std::string &path, bool httpOnly, bool secure, int maxAge, const std::string &sameSite)
{
    (void)maxAge;
    std::string out = name + "=" + value;

    if (!path.empty())
        out += "; Path=" + path;
    if (secure)
        out += "; Secure";
    if (httpOnly)
        out += "; HttpOnly";
    if (!sameSite.empty())
        out += "; SameSite=" + sameSite;

    return (out);
}
