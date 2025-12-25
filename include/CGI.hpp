/*
 * RFC 3875 The Common Gateway Interface
 *
 * The CGI is a simple interface for running external programs, software or gateways unders
 * an information server in a platform-independent manner. 
 * 
 * Supported information servers are HTTP servers.
 *
 * RFC 3986 Uniform Resource Identifier
 *
 * The URI provides a simple and extensible means for identifying a resource
 *
 */

 
#pragma once
#ifndef CGI_HPP
#define CGI_HPP

#include "Webserv.hpp"

struct ServerConf;
struct RouteConf;
struct Request;
struct Response;

#ifndef CGI_TIMEOUT
# define CGI_TIMEOUT 10
#endif

#ifndef CGI_MAX_INPUTE_BYTES
# define CGI_MAX_INPUTE_BYTES (10 * 1024 * 1024)
#endif

struct CGIProcess
{
    int             pid;
    int             inFd;
    int             outFd;
    int             errFd;
    bool            done;
    time_t          startTime;
    std::string     outBuf;
    std::string     inBuf;

    CGIProcess() : pid(-1), inFd(-1), outFd(-1), errFd(-1), done(false), startTime(0) {}
};

class CGI
{
    private:

        CGI( const CGI& obj );
        CGI& operator=( const CGI& obj );

    public:

        CGI();
        ~CGI();

        bool spawn( const ServerConf &srv, const RouteConf &loc, const Request &req, const std::string &script, CGIProcess &proc, int timeoutSecs = CGI_TIMEOUT, size_t maxInput = CGI_MAX_INPUTE_BYTES );
        bool isCGIRequest( const std::string& uri, const RouteConf& route );
        bool handleCFile( const std::string& scriptPath, std::string& excecutablePath );
        static bool collect( CGIProcess &proc );

        static void parseCgiHeaders(const std::string &raw, std::map<std::string, std::string> &h, std::string &body);

        static Response toResponse( CGIProcess &p );
        std::string getCGIExecutable( const std::string& uri, const RouteConf& route );
};

#endif
