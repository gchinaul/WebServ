
#include <fstream>
#include <algorithm>
#include <signal.h>

#include "../include/Webserv.hpp"
#include "../include/Session.hpp"

void Server::processRequest(Client &cl)
{

    const ServerConf &srv = *cl.srv;

    try
    {
        initializeSession(cl, srv);

        const RouteConf *loc = findMatchingRoute(cl, srv);
        if (!loc)
        {
            sendErrorResponse(cl, 500, "No route configured for " + cl.req.path);
            return;
        }

        ensureClientSession(cl);

        if (!validateHttpMethod(cl, srv, *loc))
            return;

        std::string fsPath = buildFilesystemPath(cl, srv, *loc);

        if (handleCGIRequest(cl, srv, *loc, fsPath))
            return;

        handleHttpMethod(cl, srv, *loc, fsPath);
        finalizeResponse(cl, srv);
    }
    catch (const std::exception &e)
    {
        handleRequestException(cl, srv, e);
    }
}
