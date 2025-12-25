
#include <fstream>
#include <algorithm>
#include <signal.h>

#include "../include/Webserv.hpp"
#include "../include/Session.hpp"

bool Server::isFdValid(int fd)
{

    if (fd < 0 || fd >= FD_SETSIZE)
        return (false);

    return (true);
}

void Server::loopStep()
{

    runSessionGC();

    fd_set readfds, writefds;
    int maxfd = buildFdSets(readfds, writefds);

    int ret = waitEvents(maxfd, readfds, writefds);

    if (ret < 0)
        return;

    if (ret > 0)
    {
        handleListenerEvents(readfds);
    }

    std::vector<int> clientFds = snapshotClientFds();
    handleClientTimeouts(clientFds);

    if (ret > 0)
    {
        processClientIO(clientFds, readfds, writefds);
    }

    checkCGITimeouts();
}

void Server::runSessionGC()
{

    static time_t lastSessionGCTime = 0;
    time_t nowClock = ft_time();

    if (nowClock - lastSessionGCTime >= 30)
    {
        SessionManager::instance().runGC();
        lastSessionGCTime = nowClock;
    }
}

bool Server::init(const std::string &confPath)
{

    if (!_cfg.load(confPath))
    {
        return (false);
    }

    _servers = _cfg.servers;

    for (size_t i = 0; i < _servers.size(); ++i)
    {

        enforceConfigLimits(_servers[i]);

        int fd;
        if (makeListener(_servers[i].host, _servers[i].port, fd))
        {

            Logger::error(ERROR_CONFIG + _servers[i].host + ":" + Libft::itoa(_servers[i].port));
            return (false);
        }
        _listenFds.push_back(fd);
        _fdToServerIdx[fd] = i;
        Logger::info(LISTENING_ON + _servers[i].host + ":" + Libft::itoa(_servers[i].port));
        Logger::info("  client_max_body: " + Libft::itoa((long)_servers[i].clientMaxBody) + " bytes");
        Logger::info("  cgi_timeout: " + Libft::itoa(_servers[i].cgiTimeout) + "s");
        Logger::info("  cgi_max_input: " + Libft::itoa((long)_servers[i].cgiMaxInput) + " bytes");
    }
    return (true);
}
