
#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#include <ctime>

#include "Config.hpp"
#include "CGI.hpp"

class ServerSocket;

#include "ServerSocket.hpp"
#include "Responder.hpp"
#include "RequestParser.hpp"

#define CLIENT_TIMEOUT 60
#define MAX_CLIENT_BODY_SIZE (1ULL * 1024 * 1024 * 1024)
#define MAX_CGI_INPUT_SIZE (100ULL * 1024 * 1024)
#define MAX_CLIENT_TIMEOUT 3600
#define MAX_CGI_TIMEOUT 300

#define DEFAULT_CLIENT_BODY_SIZE (10ULL * 1024 * 1024)

struct Client {

    std::string inBuf;
    std::string outBuf;
    std::string newSessionId;
    std::map<std::string,std::string>* sessionData;

    int fd;

    bool requestReady;
    bool usingCGI;

    const ServerConf* srv;

    Request req;
    Response resp;
    RequestParser parser;
    CGIProcess cgi;
    time_t lastActivity;

    Client():inBuf(""),outBuf(""),newSessionId(""),sessionData(NULL),fd(-1),requestReady(false),usingCGI(false),srv(0),lastActivity(time(NULL)){}
};

class Server {

    Config _cfg;

    std::map<int,size_t> _fdToServerIdx;
    std::map<int, Client> _clients;

    std::vector<int> _listenFds;
    std::vector<ServerSocket*> _serverSockets;
    std::vector<ServerConf> _servers;

    public:

        void loopStep();
        void cleanup();

        bool isFdValid( int fd );
        bool init( const std::string &confPath );

        Config& getConfig() { return _cfg; }

    private:

        std::string buildFilesystemPath( Client &cl, const ServerConf &srv, const RouteConf &loc );
        std::vector<int> snapshotClientFds() const;

        void enforceConfigLimits( ServerConf &srv );
        void runSessionGC();
        void handleListenerEvents(const fd_set &readfds);
        void handleClientTimeouts(const std::vector<int> &clientFds);
        void processClientIO(const std::vector<int> &clientFds, const fd_set &readfds, const fd_set &writefds);
        void processClientCGIIO(Client &cl, const fd_set &readfds, const fd_set &writefds);
        void checkCGITimeouts();
        void addClient( int cfd, size_t srvIdx );
        void closeClient( int fd );
        void resetClient( Client &cl );
        void handleReadable( int fd );
        bool handleNewConnection( int fd );
        void acceptPendingConnections( int listenerFd );
        void handleClientRead( int fd );
        void processReceivedData( Client &cl, const char *data, size_t len );
        void handleParsingError( Client &cl, int errCode );
        void sendClientData( Client &cl, int fd );
        void handleWritable( int fd );
        void processRequest( Client &cl );
        void initializeSession( Client &cl, const ServerConf &srv );
        void sortRoutesBySpecificity( std::vector<size_t> &idxs, const std::vector<RouteConf> &routes );
        void ensureClientSession( Client &cl );
        void send405Response( Client &cl, const ServerConf &srv, const RouteConf &loc );
        void handleHttpMethod( Client &cl, const ServerConf &srv, const RouteConf &loc, const std::string &fsPath );
        void finalizeResponse( Client &cl, const ServerConf &srv );
        void sendErrorResponse( Client &cl, int code, const std::string &logMsg );
        void addSessionCookie( Client &cl, const ServerConf &srv, std::string &response );
        void handleRequestException( Client &cl, const ServerConf &srv, const std::exception &e );
        
        bool handleCGIRequest( Client &cl, const ServerConf &srv, const RouteConf &loc, const std::string &fsPath );
        bool routeMatches( const std::string &reqPath, const std::string &routePath );
        bool validateHttpMethod( Client &cl, const ServerConf &srv, const RouteConf &loc );

        int makeListener( const std::string& host, uint16_t port, int& fd );
        int buildFdSets(fd_set &readfds, fd_set &writefds);
        int waitEvents(int maxfd, fd_set &readfds, fd_set &writefds);
        
        const RouteConf* findMatchingRoute( Client &cl, const ServerConf &srv );
};

#endif