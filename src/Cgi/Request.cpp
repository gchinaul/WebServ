
#include <fstream>
#include <algorithm>
#include <signal.h>

#include "../include/Webserv.hpp"
#include "../include/Session.hpp"

bool Server::handleCGIRequest( Client &cl, const ServerConf &srv, const RouteConf &loc, const std::string &fsPath ) {

    if ( cl.req.method == "DELETE" || !loc.hasCGI )
        return ( false );

    size_t dot = fsPath.find_last_of( '.' );
    if ( dot == std::string::npos )
        return ( false );

    std::string ext = fsPath.substr( dot );
    if ( loc.cgiMap.find( ext ) == loc.cgiMap.end() )
        return ( false );

    Logger::debug( "Starting CGI for " + fsPath );
    
    CGI cgi;
    if ( !cgi.spawn( srv, loc, cl.req, fsPath, cl.cgi, srv.cgiTimeout, srv.cgiMaxInput ) ) {
        Logger::error( "CGI spawn failed for " + fsPath );
        sendErrorResponse( cl, 500, "CGI spawn failed" );
        return ( true );
    }

    cl.usingCGI = true;
    return ( true );
}
