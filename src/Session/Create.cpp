
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <algorithm>
#include <iostream>
#include <sys/stat.h>
#include <fstream>

#include "../include/Session.hpp"
#include "../include/LibftCpp.hpp"
#include "../include/Time.hpp"

#ifdef USE_SQLITE
#include <sqlite3.h>
#endif

std::string SessionManager::createSession()
{

    runGC();

    const size_t bytes = 32;
    unsigned char buf[bytes];

    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0)
    {

        std::string fallback;
        static const char *hex = "0123456789abcdef";

        for (size_t i = 0; i < bytes * 2; ++i)
            fallback += hex[Libft::ft_rand() % 16];

        std::string id = fallback;
        SessionEntry e;
        e.data = std::map<std::string, std::string>();
        e.created = ft_time();
        e.lastAccess = e.created;
        sessions[id] = e;
        sessions[id].data["created"] = Libft::itoa((long)e.created);

        return (id);
    }

    ssize_t r = read(fd, buf, bytes);
    close(fd);
    std::string id;
    static const char *hex = "0123456789abcdef";

    if (r == (ssize_t)bytes)
    {

        for (size_t i = 0; i < bytes; ++i)
        {

            unsigned char v = buf[i];
            id += hex[(v >> 4) & 0xF];
            id += hex[v & 0xF];
        }
    }
    else
    {
        for (size_t i = 0; i < bytes * 2; ++i)
            id += hex[Libft::ft_rand() % 16];
    }

    SessionEntry e;
    e.data = std::map<std::string, std::string>();
    e.created = ft_time();
    e.lastAccess = e.created;
    sessions[id] = e;
    sessions[id].data["created"] = Libft::itoa((long)e.created);

    return (id);
}
