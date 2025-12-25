
#pragma once
#ifndef TCP_CONNECTION_HPP
# define TCP_CONNECTION_HPP

# include <unistd.h>
# include <string>
# include <sys/socket.h>
# include <netinet/in.h>

# include "ASocket.hpp"
# include "Logger.hpp"

# define PORT 8080

class TCPConnection : public ASocket {

    private:

        int _connectedFd;

        TCPConnection( const TCPConnection& obj );  
        TCPConnection& operator=( const TCPConnection& obj );

        int     createSocket();
        int     connectToServer();
        int     setLocalConnection();
        int     setRemoteConnection();
        void    closeConnectedFd();

    public:

        TCPConnection();
        TCPConnection( t_param param );
        TCPConnection( int clientFd, sockaddr_in clientAddress );
        ~TCPConnection();

        int     getConnectedFd() const;

};

#endif