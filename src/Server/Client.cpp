
#include <fstream>
#include <algorithm>
#include <signal.h>

#include "../include/Webserv.hpp"
#include "../include/Session.hpp"

void Server::addClient(int cfd, size_t srvIdx)
{

    Client cl;
    cl.fd = cfd;
    cl.srv = &_servers[srvIdx];
    _clients[cfd] = cl;
}

void Server::closeClient(int fd)
{

    std::map<int, Client>::iterator it = _clients.find(fd);

    if (it != _clients.end())
    {
        Logger::debug("Closing client FD: " + Libft::itoa(fd));

        if (it->second.usingCGI)
        {
            if (it->second.cgi.inFd != -1)
            {
                close(it->second.cgi.inFd);
                it->second.cgi.inFd = -1;
            }
            if (it->second.cgi.outFd != -1)
            {
                close(it->second.cgi.outFd);
                it->second.cgi.outFd = -1;
            }
            if (it->second.cgi.errFd != -1)
            {
                close(it->second.cgi.errFd);
                it->second.cgi.errFd = -1;
            }
            it->second.usingCGI = false;
        }
        close(fd);
        _clients.erase(it);

        Logger::debug("Client FD " + Libft::itoa(fd) + " successfully closed and removed");
    }
    else
    {
        Logger::debug("Attempted to close non-existent client FD: " + Libft::itoa(fd));
    }
}

void Server::resetClient(Client &cl)
{

    int fd = cl.fd;
    const ServerConf* srv = cl.srv;

    cl.inBuf.clear();
    cl.outBuf.clear();
    cl.newSessionId.clear();
    cl.sessionData = NULL;
    cl.requestReady = false;
    cl.usingCGI = false;
    cl.req = Request();
    cl.resp = Response();
    cl.parser = RequestParser();
    cl.lastActivity = time(NULL);
    cl.fd = fd;
    cl.srv = srv;

    Logger::debug("Client FD " + Libft::itoa(fd) + " reset for keep-alive");
}

void Server::handleClientRead(int fd)
{

    std::map<int, Client>::iterator it = _clients.find(fd);
    if (it == _clients.end())
        return;

    Client &cl = it->second;
    cl.lastActivity = ft_time();

    char buf[4096];
    ssize_t n = recv(fd, buf, sizeof(buf), 0);

    if (n < 0)
    {
        Logger::error("recv() error on client fd=" + Libft::itoa(fd));
        closeClient(fd);
        return;
    }
    else if (n == 0)
    {
        Logger::debug("Client disconnected properly (fd=" + Libft::itoa(fd) + ")");
        closeClient(fd);
        return;
    }

    processReceivedData(cl, buf, (size_t)n);
}

std::vector<int> Server::snapshotClientFds() const
{

    std::vector<int> fds;
    fds.reserve(_clients.size());
    for (std::map<int, Client>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
        fds.push_back(it->first);
    return (fds);
}

void Server::handleClientTimeouts(const std::vector<int> &clientFds)
{

    time_t now = ft_time();
    for (size_t i = 0; i < clientFds.size(); ++i)
    {
        int fd = clientFds[i];
        std::map<int, Client>::iterator it = _clients.find(fd);
        if (it != _clients.end())
        {
            if (now - it->second.lastActivity > CLIENT_TIMEOUT)
            {
                Logger::info("Client timeout (fd=" + Libft::itoa(fd) + ", inactive for " +
                             Libft::itoa(now - it->second.lastActivity) + "s)");
                closeClient(fd);
            }
        }
    }
}

void Server::ensureClientSession(Client &cl)
{

    std::map<std::string, std::string> cookies;
    std::map<std::string, std::string>::const_iterator cit = cl.req.headers.find("cookie");

    if (cit != cl.req.headers.end())
        cookies = Libft::parseCookies(cit->second);

    std::map<std::string, std::string>::const_iterator got = cookies.find("SID");

    if (got != cookies.end())
    {
        std::map<std::string, std::string> *s = SessionManager::instance().getSession(got->second);
        if (s)
            cl.sessionData = s;
        else
        {
            std::string sid = SessionManager::instance().createSession();
            cl.newSessionId = sid;
            cl.sessionData = SessionManager::instance().getSession(sid);
        }
    }
    else
    {
        std::string sid = SessionManager::instance().createSession();
        cl.newSessionId = sid;
        cl.sessionData = SessionManager::instance().getSession(sid);
    }
}
