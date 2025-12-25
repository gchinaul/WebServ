
#include "../include/ServerSocket.hpp"
#include "../include/Message.hpp"
#include <sys/socket.h>
#include <errno.h>
#include <string.h>

ServerSocket::ServerSocket() : ASocket(), _client(NULL), _listenerFd(-1)
{

    if (createSocket() < 0)
        return;
    if (!setListener())
        return;
}

ServerSocket::ServerSocket(t_param p) : ASocket(p), _client(NULL), _listenerFd(-1)
{

    if (createSocket() < 0)
        return;
    if (!setListener())
        return;
}

ServerSocket::ServerSocket(const ServerSocket &obj) : ASocket(obj)
{

    (void)obj;
}

ServerSocket &ServerSocket::operator=(const ServerSocket &obj)
{

    (void)obj;
    return (*this);
}

ServerSocket::~ServerSocket()
{

    closeListenerFd();
}

int ServerSocket::getListenerFd() const
{

    return (_listenerFd);
}

int ServerSocket::setListener()
{

    if (!bindSocket() || !startListening())
    {

        closeListenerFd();
        _errorCode = 1;
        return (0);
    }
    return (1);
}

int ServerSocket::createSocket()
{

    _listenerFd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);

    if (_listenerFd < 0)
    {

        Logger::error(ERROR_SOCKET);
        _errorCode = 1;
        return (-1);
    }

    int opt = 1;
    if (setsockopt(_listenerFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        Logger::error("setsockopt(SO_REUSEADDR) failed");
    }

    Logger::info("Socket has been successfully created");

    return (0);
}

int ServerSocket::bindSocket()
{

    socklen_t len = sizeof(_localAddress);

    if (bind(_listenerFd, reinterpret_cast<struct sockaddr *>(&_localAddress), len) < 0)
    {

        Logger::error(ERROR_BIND);
        _errorCode = 1;
        return (0);
    }
    Logger::info("Socket has been bound");
    return (1);
}

int ServerSocket::startListening()
{

    if (listen(_listenerFd, SOMAXCONN) < 0)
    {

        Logger::error(ERROR_LISTEN);
        _errorCode = 1;
        return (0);
    }
    Logger::info("Server is listening");
    return (1);
}

int ServerSocket::acceptConnection()
{

    sockaddr_in clientAddress = sockaddr_in();
    socklen_t len = sizeof(clientAddress);

    int clientFd = accept(_listenerFd, reinterpret_cast<sockaddr *>(&clientAddress), &len);

    if (clientFd < 0)
    {

        Logger::error(std::string(ERROR_ACCEPT));
        _errorCode = 1;
        return (0);
    }
    else
    {

        _client = new TCPConnection(clientFd, clientAddress);
        Logger::info("Server accepted a client connection");
        return (1);
    }
}

void ServerSocket::closeListenerFd()
{

    if (_listenerFd >= 0)
    {

        close(_listenerFd);
        _listenerFd = -1;
    }
}

void ServerSocket::closeTCPConnection()
{

    if (_client != NULL)
        delete _client;
}
