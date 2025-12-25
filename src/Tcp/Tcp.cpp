
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "LibftCpp.hpp"
#include "TCPConnection.hpp"

TCPConnection::TCPConnection() : ASocket(), _connectedFd( -1 ) {

    if ( createSocket() < 0 )
        return;
    if ( !connectToServer() )
        return;
}

TCPConnection::TCPConnection( t_param p ) : ASocket( p ), _connectedFd( -1 ) {

    if ( createSocket() < 0 )
        return;
    if ( !connectToServer() )
        return;
}

TCPConnection::TCPConnection( int clientFd, sockaddr_in clientAddress ) : ASocket(), _connectedFd( clientFd ) {

    _localAddress = clientAddress;
    Logger::info( "TCPConnection has been created from accepted client socket" );
}

TCPConnection::TCPConnection( const TCPConnection& obj ) : ASocket( obj ) {

    (void)obj;
}

TCPConnection& TCPConnection::operator=( const TCPConnection& obj ) {

    (void)obj;
    return ( *this );
}

TCPConnection::~TCPConnection() {

    if ( _connectedFd != -1 )
        closeConnectedFd();
}

int     TCPConnection::getConnectedFd() const {

    return ( _connectedFd );
}

int     TCPConnection::createSocket() {

    #if defined(__linux__)
        _connectedFd = socket( AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0 );
    #else
        _connectedFd = socket( AF_INET, SOCK_STREAM, 0 );
    #endif
    
    if ( _connectedFd < 0 ) {
        
        Logger::error( "cannot create socket" );
        _errorCode = 1;
        return ( -1 );
    }
    Logger::info( "Socket has been successfully created" );
    
    return ( 0 );
}

int     TCPConnection::connectToServer() {
    
    if ( _host == "localhost" ) {
        
        return ( setLocalConnection() );
    }
    else if ( _host.empty() || _host == "*" || _host == "0.0.0.0" ) {
        
        Logger::error( "makeConnectionRequest: invalid remote host: " + _host );
        _errorCode = 1;
        return ( 0 );
    }
    else {
        
        return ( setRemoteConnection() );

    }
}

int     TCPConnection::setLocalConnection() {

    struct sockaddr_in serverAddress = sockaddr_in();
    serverAddress.sin_addr.s_addr = htonl( INADDR_LOOPBACK );
    
    if ( connect( _connectedFd, reinterpret_cast<struct sockaddr *>( &serverAddress ), sizeof( serverAddress ) ) < 0 ) {
    
        Logger::error( "in makeConnectionRequest()" );
        _errorCode = 1;
        return ( 0 );
    }
    Logger::info( "Client is connected" );
    return ( 1 );
}

int     TCPConnection::setRemoteConnection() {

        struct addrinfo    hints;
        struct addrinfo*   res;
        int                status;
        
        Libft::ft_memset( &hints, 0, sizeof( hints ) );
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = 0;
        
        status = getaddrinfo( _host.c_str(), "8080", &hints, &res );
        if ( status != 0 ) {
            
            Logger::error( std::string( "Error on getaddrinfo: " ) + gai_strerror( status ) );
            _errorCode = 1;
            return ( 0 );
        }
    
        int result = connect( _connectedFd, res->ai_addr, res->ai_addrlen );
        freeaddrinfo( res );
        if ( result < 0 ) {
            Logger::error( "connect() failed" );
            _errorCode = 1;
            return ( 0 );
        }
        Logger::info( "Client is connected" );
        return ( 1 );
}

void    TCPConnection::closeConnectedFd() {

    if ( _connectedFd >= 0 ) {
    
        close( _connectedFd );
        _connectedFd = -1;
    }
}
