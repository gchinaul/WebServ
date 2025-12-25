#include <sys/resource.h>
#include <fcntl.h>
#include <fstream>
#include <limits.h>

#include "../include/Webserv.hpp"

bool CGI::collect(CGIProcess &p)
{

    char buf[4096];
    ssize_t n = read(p.outFd, buf, sizeof(buf));
    if (n < 0)
    {
        Logger::error("error cgi read");
        p.done = true;
        return (true);
    }
    else if (n > 0)
    {
        p.outBuf.append(buf, n);
        return (false);
    }
    else
    {
        p.done = (true);
        return (true);
    }
}
