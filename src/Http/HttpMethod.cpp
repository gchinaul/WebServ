
#include <fstream>
#include <algorithm>
#include <signal.h>

#include "../include/Webserv.hpp"
#include "../include/Session.hpp"

void Server::handleHttpMethod(Client &cl, const ServerConf &srv, const RouteConf &loc, const std::string &fsPath)
{

    Logger::debug("Processing HTTP method: " + cl.req.method + " for path: " + fsPath);

    if (cl.req.method == "GET" || cl.req.method == "HEAD")
    {
        cl.resp = Responder::handleGet(srv, loc, cl.req);
        if (cl.req.method == "HEAD")
        {
            if (cl.resp.headers.find("Content-Length") == cl.resp.headers.end())
                cl.resp.headers["Content-Length"] = Libft::itoa((long)cl.resp.body.size());
            cl.resp.body.clear();
        }
    }
    else if (cl.req.method == "POST")
    {
        if (cl.req.path == "/from")
        {
            cl.resp.status = 200;
            cl.resp.body = "<html><body>POST /from OK</body></html>";
            cl.resp.headers["Content-Type"] = "text/html";
            cl.resp.headers["Content-Length"] = Libft::itoa((long)cl.resp.body.size());
        }
        else
            cl.resp = Responder::handlePost(srv, loc, cl.req);
    }
    else if (cl.req.method == "DELETE")
        cl.resp = Responder::handleDelete(srv, loc, cl.req);
    else
    {
        Logger::error("Unsupported HTTP method: " + cl.req.method);
        cl.resp = Responder::makeError(srv, 405);
    }
}

bool Server::validateHttpMethod(Client &cl, const ServerConf &srv, const RouteConf &loc)
{

    std::string allowed = "";
    for (std::set<std::string>::const_iterator it = loc.allowedMethods.begin(); it != loc.allowedMethods.end(); ++it)
    {
        if (!allowed.empty())
            allowed += ",";
        allowed += *it;
    }
    Logger::debug("Allowed methods for route '" + loc.path + "': " + allowed);

    if (loc.allowedMethods.find(cl.req.method) != loc.allowedMethods.end())
        return (true);

    Logger::error("Method " + cl.req.method + " not allowed for " + cl.req.path);
    send405Response(cl, srv, loc);
    return (false);
}
