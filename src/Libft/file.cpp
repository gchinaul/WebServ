
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>

#include "../include/Webserv.hpp"

bool Libft::fileExists(const std::string &path)
{

    std::ifstream f(path.c_str());

    return (f.good());
}

std::string Libft::readFile(const std::string &path, bool *ok)
{

    std::ifstream f(path.c_str(), std::ios::in | std::ios::binary);
    if (!f)
    {

        if (ok)
            *ok = false;
        return ("");
    }

    std::ostringstream ss;
    ss << f.rdbuf();

    if (ok)
        *ok = true;

    return (ss.str());
}

bool Libft::writeFile(const std::string &path, const std::string &data)
{

    std::ofstream f(path.c_str(), std::ios::out | std::ios::binary);
    if (!f)
        return (false);

    f.write(data.c_str(), (std::streamsize)data.size());

    return ((bool)f);
}
