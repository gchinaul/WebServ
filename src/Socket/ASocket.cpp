
#include <arpa/inet.h>

#include "../include/ASocket.hpp"
#include "../include/LibftCpp.hpp"

ASocket::ASocket() : _host("localhost")
{

    _errorCode = 0;

    _localAddress = sockaddr_in();
    _localAddress.sin_family = AF_INET;
    _localAddress.sin_port = htons(PORT);
    _localAddress.sin_addr.s_addr = INADDR_ANY;
}

ASocket::ASocket(t_param p) : _host(p.host)
{

    _errorCode = 0;

    _localAddress = sockaddr_in();
    _localAddress.sin_family = p.domain;
    _localAddress.sin_port = htons(p.port);

    if (!p.host.empty() && p.host != "0.0.0.0" && p.host != "localhost")
    {

        in_addr_t addr = Libft::ft_inet_addr(p.host.c_str());

        if (addr != INADDR_NONE)
            _localAddress.sin_addr.s_addr = addr;
        else
            _localAddress.sin_addr.s_addr = INADDR_ANY;
    }
    else
        _localAddress.sin_addr.s_addr = INADDR_ANY;
}

ASocket::ASocket(const ASocket &obj)
{

    (void)obj;
}

ASocket &ASocket::operator=(const ASocket &obj)
{

    (void)obj;
    return (*this);
}

ASocket::~ASocket() {}

int ASocket::getErrorCode() const
{

    return (_errorCode);
}
