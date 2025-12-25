
#pragma once
#ifndef TCP_SERVER_SOCKET_HPP
# define TCP_SERVER_SOCKET_HPP

# include <unistd.h>
# include <string>
# include <sys/socket.h>
# include <netinet/in.h>

# include "ASocket.hpp"
# include "TCPConnection.hpp"
# include "../include/Logger.hpp"

# define PORT 8080

class ServerSocket : public ASocket {

    private:

        TCPConnection*      _client;

        ServerSocket( const ServerSocket& obj );  
        ServerSocket& operator=( const ServerSocket& obj );

        int     setListener();
        int     createSocket();
        int     bindSocket();
        int     startListening();
        int     acceptConnection();
        int     _listenerFd;

        void    closeListenerFd();
        void    closeTCPConnection();

    public:

        ServerSocket();
        ServerSocket( t_param param );
        ~ServerSocket();

        int getListenerFd() const;
};

#endif