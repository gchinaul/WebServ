
#include <fstream>
#include <algorithm>
#include <signal.h>

#include "../include/Webserv.hpp"
#include "../include/Session.hpp"

int Server::buildFdSets(fd_set &readfds, fd_set &writefds)
{

    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    int maxfd = 0;

    for (size_t i = 0; i < _listenFds.size(); ++i)
    {
        FD_SET(_listenFds[i], &readfds);
        if (_listenFds[i] > maxfd)
            maxfd = _listenFds[i];
    }

    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        int fd = it->first;
        if (!isFdValid(fd))
        {
            Logger::warning("Skipping invalid client FD during FD_SET: " + Libft::itoa(fd));
            continue;
        }
        if (fd >= FD_SETSIZE)
        {
            Logger::error("Client fd >= FD_SETSIZE. Closing to avoid select overflow: " + Libft::itoa(fd));
            closeClient(fd);
            continue;
        }
        FD_SET(fd, &readfds);
        if (!it->second.outBuf.empty())
            FD_SET(fd, &writefds);
        if (fd > maxfd)
            maxfd = fd;

        if (it->second.usingCGI)
        {
            if (isFdValid(it->second.cgi.outFd))
            {
                FD_SET(it->second.cgi.outFd, &readfds);
                if (it->second.cgi.outFd > maxfd)
                    maxfd = it->second.cgi.outFd;
            }
            if (isFdValid(it->second.cgi.errFd))
            {
                FD_SET(it->second.cgi.errFd, &readfds);
                if (it->second.cgi.errFd > maxfd)
                    maxfd = it->second.cgi.errFd;
            }
            if (isFdValid(it->second.cgi.inFd) && !it->second.cgi.inBuf.empty())
            {
                FD_SET(it->second.cgi.inFd, &writefds);
                if (it->second.cgi.inFd > maxfd)
                    maxfd = it->second.cgi.inFd;
            }
        }
    }
    return (maxfd);
}
