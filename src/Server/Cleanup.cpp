
#include <fstream>
#include <algorithm>
#include <signal.h>

#include "../include/Webserv.hpp"
#include "../include/Session.hpp"

void Server::cleanup()
{

    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {

        if (it->second.fd != -1)
            close(it->second.fd);
    }
    _clients.clear();
    std::map<int, Client>().swap(_clients);

    for (size_t i = 0; i < _listenFds.size(); ++i)
    {

        if (_listenFds[i] != -1)
            close(_listenFds[i]);
    }
    _listenFds.clear();
    std::vector<int>().swap(_listenFds);

    for (size_t i = 0; i < _serverSockets.size(); ++i)
    {

        if (_serverSockets[i])
            delete _serverSockets[i];
    }
    _serverSockets.clear();
    std::vector<ServerSocket *>().swap(_serverSockets);
    _fdToServerIdx.clear();
    std::map<int, size_t>().swap(_fdToServerIdx);
    _servers.clear();
    std::vector<ServerConf>().swap(_servers);
}
