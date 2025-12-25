
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>

#include "../include/Webserv.hpp"

bool Libft::isDir(const std::string &path)
{

    struct stat st;

    if (stat(path.c_str(), &st) != 0)
        return (false);

    return (S_ISDIR(st.st_mode));
}
