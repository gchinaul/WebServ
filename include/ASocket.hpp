/*
 * RFC 147 Definition of a Socket
 * 
 * A Socket is defined to be the unique identification to or from wich information 
 * is transmitted in the network. The socket as specified as a 32 bit number with even
 * sockets.
 *
 * Socket also identified by the host in wich the sending or receiving processer is located
 */

#pragma once
#ifndef ASOCKET_HPP
#define ASOCKET_HPP

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>

# define PORT 8080

typedef struct s_param
{
    int             domain;
    int             type;
    int             protocol;
    int             port;
    std::string     host;
}               t_param;

class ASocket
{
    protected:

        int                 _errorCode;

        const std::string   _host;
        struct sockaddr_in  _localAddress;

        ASocket( const ASocket& obj );
        ASocket& operator=( const ASocket& obj );

    public:

        ASocket();
        ASocket( t_param param );
        virtual ~ASocket();

        int getErrorCode() const;
};

#endif
