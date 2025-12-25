
#include <fstream>
#include <algorithm>
#include <signal.h>

#include "../include/Webserv.hpp"
#include "../include/Session.hpp"

int Server::waitEvents(int maxfd, fd_set &readfds, fd_set &writefds)
{

    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    return (select(maxfd + 1, &readfds, &writefds, NULL, &tv));
}

void Server::handleListenerEvents(const fd_set &readfds)
{

    for (size_t i = 0; i < _listenFds.size(); ++i)
    {
        if (FD_ISSET(_listenFds[i], &readfds))
            handleReadable(_listenFds[i]);
    }
}

int Server::makeListener(const std::string &host, uint16_t port, int &fd)
{

    t_param p;
    p.host = host;
    p.domain = AF_INET;
    p.type = SOCK_STREAM;
    p.protocol = 0;
    p.port = port;

    ServerSocket *serverSocket = new ServerSocket(p);

    if (serverSocket->getErrorCode() != 0)
    {

        delete serverSocket;
        Logger::error("Listener could not be set.");
        return (-1);
    }

    fd = serverSocket->getListenerFd();
    _serverSockets.push_back(serverSocket);
    return (0);
}

void Server::enforceConfigLimits(ServerConf &srv)
{

    if (srv.clientMaxBody > MAX_CLIENT_BODY_SIZE)
    {
        Logger::warning("Server " + srv.serverName +
                        ": client_max_body " + Libft::itoa((long)srv.clientMaxBody) +
                        " exceeds hard limit. Capping to " +
                        Libft::itoa((long)MAX_CLIENT_BODY_SIZE) + " bytes");
        srv.clientMaxBody = MAX_CLIENT_BODY_SIZE;
    }

    if (srv.cgiMaxInput > MAX_CGI_INPUT_SIZE)
    {
        Logger::warning("Server " + srv.serverName +
                        ": cgi_max_input " + Libft::itoa((long)srv.cgiMaxInput) +
                        " exceeds hard limit. Capping to " +
                        Libft::itoa((long)MAX_CGI_INPUT_SIZE) + " bytes");
        srv.cgiMaxInput = MAX_CGI_INPUT_SIZE;
    }

    if (srv.cgiTimeout > MAX_CGI_TIMEOUT)
    {
        Logger::warning("Server " + srv.serverName +
                        ": cgi_timeout " + Libft::itoa(srv.cgiTimeout) +
                        "s exceeds hard limit. Capping to " +
                        Libft::itoa(MAX_CGI_TIMEOUT) + "s");
        srv.cgiTimeout = MAX_CGI_TIMEOUT;
    }
}

void Server::handleReadable(int fd)
{

    if (handleNewConnection(fd))
        return;

    handleClientRead(fd);
}

bool Server::handleNewConnection(int fd)
{

    for (size_t i = 0; i < _listenFds.size(); ++i)
    {
        if (fd == _listenFds[i])
        {
            acceptPendingConnections(fd);
            return (true);
        }
    }
    return (false);
}

void Server::acceptPendingConnections(int listenerFd)
{

    while (true)
    {
        sockaddr_in caddr;
        socklen_t clen = sizeof(caddr);
        int cfd = accept(listenerFd, (sockaddr *)&caddr, &clen);

        if (cfd >= 0)
        {
            addClient(cfd, _fdToServerIdx[listenerFd]);
            Logger::debug("New client connect from " + std::string(Libft::ft_inet_ntoa(caddr.sin_addr)));
        }
        else if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            break;
        }
        else
        {
            Logger::error("accept() error on listener fd=" + Libft::itoa(listenerFd) + ": " + strerror(errno));
            break;
        }
    }
}

void Server::processReceivedData(Client &cl, const char *data, size_t len)
{

    int err = 0;

    if (cl.parser.feed(data, len, cl.srv->clientMaxBody, err))
    {
        cl.requestReady = true;
        cl.req = cl.parser.get();

        sockaddr_in caddr;
        std::string clientIP = "unknown";
        clientIP = std::string(Libft::ft_inet_ntoa(caddr.sin_addr));

        Logger::info("[" + clientIP + "] " + cl.req.method + " " + cl.req.uri);

        processRequest(cl);
    }
    else if (err != 0)
    {
        handleParsingError(cl, err);
    }
}

void Server::handleParsingError(Client &cl, int errCode)
{

    Logger::error("Request parsing error: " + Libft::itoa(errCode));

    cl.resp = Responder::makeError(*cl.srv, errCode);

    if (!cl.newSessionId.empty() && cl.resp.headers.find("Set-Cookie") == cl.resp.headers.end())
    {
        const ServerConf &tsrv = *cl.srv;
        unsigned int ttl = tsrv.sessionTTL ? tsrv.sessionTTL : SessionManager::instance().getTTL();
        cl.resp.headers["Set-Cookie"] = Libft::makeSetCookie("SID", cl.newSessionId, "/", true, tsrv.cookieSecure, (int)ttl, tsrv.cookieSameSite);
        cl.newSessionId.clear();
    }

    cl.outBuf = cl.resp.serialize();
}

void Server::handleWritable(int fd)
{

    std::map<int, Client>::iterator it = _clients.find(fd);
    if (it == _clients.end())
        return;

    Client &cl = it->second;
    cl.lastActivity = ft_time();

    if (!cl.outBuf.empty())
        sendClientData(cl, fd);
}

void Server::sendClientData(Client &cl, int fd)
{

    ssize_t n = send(fd, cl.outBuf.data(), cl.outBuf.size(), 0);

    if (n < 0)
    {
        Logger::error("send() error on client fd=" + Libft::itoa(fd));
        closeClient(fd);
        return;
    }

    if (n > 0)
        cl.outBuf.erase(0, (size_t)n);

    if (cl.outBuf.empty())
    {
        sockaddr_in caddr;
        std::string clientIP = "unknown";
        clientIP = std::string(Libft::ft_inet_ntoa(caddr.sin_addr));

        Logger::info("[" + clientIP + "] Response: " + Libft::itoa(cl.resp.status));

        std::map<std::string, std::string>::const_iterator connIt = cl.req.headers.find("connection");
        bool clientWantsClose = (connIt != cl.req.headers.end() &&
                                 (connIt->second == "close" || connIt->second == "Close"));

        if (clientWantsClose || cl.resp.status >= 400)
        {
            closeClient(fd);
        }
        else
        {
            resetClient(cl);
        }
    }
}

void Server::initializeSession(Client &cl, const ServerConf &srv)
{

    try
    {
        if (srv.sessionTTL > 0)
            SessionManager::instance().setTTL(srv.sessionTTL);

        if (!srv.sessionPersist.empty())
            SessionManager::instance().enablePersistence(srv.sessionPersist);

        cl.sessionData = NULL;

        std::map<std::string, std::string>::const_iterator cit = cl.req.headers.find("cookie");
        if (cit == cl.req.headers.end())
            return;

        std::map<std::string, std::string> cookies = Libft::parseCookies(cit->second);
        std::map<std::string, std::string>::const_iterator got = cookies.find("SID");

        if (got == cookies.end())
            got = cookies.find("sid");

        if (got != cookies.end())
        {
            std::map<std::string, std::string> *sess = SessionManager::instance().getSession(got->second);
            if (sess)
                cl.sessionData = sess;
        }
    }
    catch (...)
    {
        Logger::warning("Session handling threw an exception");
    }
}

const RouteConf *Server::findMatchingRoute(Client &cl, const ServerConf &srv)
{

    std::vector<size_t> idxs;
    for (size_t i = 0; i < srv.routes.size(); ++i)
        idxs.push_back(i);

    sortRoutesBySpecificity(idxs, srv.routes);

    for (size_t ii = 0; ii < idxs.size(); ++ii)
    {
        const RouteConf &r = srv.routes[idxs[ii]];
        if (routeMatches(cl.req.path, r.path))
        {
            Logger::debug("Processing " + cl.req.method + " " + cl.req.path + " -> route: " + r.path);
            return (&r);
        }
    }

    if (srv.routes.size() > 0)
        return (&srv.routes[0]);

    return (NULL);
}

void Server::sortRoutesBySpecificity(std::vector<size_t> &idxs, const std::vector<RouteConf> &routes)
{

    for (size_t i = 0; i < idxs.size(); ++i)
    {
        size_t best = i;
        for (size_t j = i + 1; j < idxs.size(); ++j)
        {
            if (routes[idxs[j]].path.size() > routes[idxs[best]].path.size())
                best = j;
        }
        if (best != i)
            std::swap(idxs[i], idxs[best]);
    }
}

bool Server::routeMatches(const std::string &reqPath, const std::string &routePath)
{

    if (Libft::startsWith(reqPath, routePath))
        return (true);

    if (!routePath.empty() && routePath[routePath.size() - 1] == '/')
    {
        if (routePath.size() > 0 && reqPath == routePath.substr(0, routePath.size() - 1))
            return (true);

        std::string reqPlusSlash = reqPath;
        if (reqPlusSlash.empty() || reqPlusSlash[reqPlusSlash.size() - 1] != '/')
            reqPlusSlash += '/';
        if (Libft::startsWith(reqPlusSlash, routePath))
            return (true);
    }

    return (false);
}

void Server::send405Response(Client &cl, const ServerConf &srv, const RouteConf &loc)
{

    std::string allowHeader = "";
    for (std::set<std::string>::const_iterator it = loc.allowedMethods.begin(); it != loc.allowedMethods.end(); ++it)
    {
        if (!allowHeader.empty())
            allowHeader += ", ";
        allowHeader += *it;
    }

    std::string errorPath = srv.errorDir + "/405.html";
    std::ifstream errorFile(errorPath.c_str());
    std::string errorBody;

    if (errorFile.is_open())
    {
        std::string line;
        while (std::getline(errorFile, line))
            errorBody += line + "\n";
        errorFile.close();
    }
    else
        errorBody = "<html><body><h1>405 Method Not Allowed</h1></body></html>";

    std::string response = "HTTP/1.1 405 Method Not Allowed\r\n";
    response += "Content-Type: text/html\r\n";
    response += "Allow: " + allowHeader + "\r\n";
    response += "Content-Length: " + Libft::itoa((long)errorBody.size()) + "\r\n";
    response += "Connection: keep-alive\r\n";

    addSessionCookie(cl, srv, response);

    response += "\r\n";
    response += errorBody;
    cl.outBuf = response;
}

std::string Server::buildFilesystemPath(Client &cl, const ServerConf &srv, const RouteConf &loc)
{

    std::string rel;
    if (cl.req.path.size() >= loc.path.size())
        rel = cl.req.path.substr(loc.path.size());
    else
        rel = "";

    if (!rel.empty() && rel[0] == '/')
        rel = rel.substr(1);

    std::string fsPath;
    if (loc.root.empty())
    {
        fsPath = srv.root;
        if (!fsPath.empty() && fsPath[fsPath.size() - 1] != '/')
            fsPath += '/';
        fsPath += cl.req.path.substr(1);
    }
    else
    {
        fsPath = loc.root;
        if (!fsPath.empty() && fsPath[fsPath.size() - 1] != '/')
            fsPath += '/';
        fsPath += rel;
    }

    return (fsPath);
}

void Server::finalizeResponse(Client &cl, const ServerConf &srv)
{

    if (!cl.newSessionId.empty() && cl.resp.headers.find("Set-Cookie") == cl.resp.headers.end())
    {
        unsigned int ttl = srv.sessionTTL ? srv.sessionTTL : SessionManager::instance().getTTL();
        cl.resp.headers["Set-Cookie"] = Libft::makeSetCookie("SID", cl.newSessionId, "/", true, srv.cookieSecure, (int)ttl, srv.cookieSameSite);
        cl.newSessionId.clear();
    }

    cl.outBuf = cl.resp.serialize();
}

void Server::sendErrorResponse(Client &cl, int code, const std::string &logMsg)
{

    if (!logMsg.empty())
        Logger::error(logMsg);

    std::string errorPage = "HTTP/1.1 " + Libft::itoa(code) + " " + HTTP::reason(code) + "\r\n";
    errorPage += "Content-Type: text/html\r\n";
    errorPage += "Content-Length: 60\r\n";
    errorPage += "Connection: keep-alive\r\n";

    addSessionCookie(cl, *cl.srv, errorPage);

    errorPage += "\r\n";
    errorPage += "<html><body><h1>" + Libft::itoa(code) + " " + HTTP::reason(code) + "</h1></body></html>";
    cl.outBuf = errorPage;
}

void Server::addSessionCookie(Client &cl, const ServerConf &srv, std::string &response)
{

    if (!cl.newSessionId.empty())
    {
        unsigned int ttl = srv.sessionTTL ? srv.sessionTTL : SessionManager::instance().getTTL();
        response += "Set-Cookie: " + Libft::makeSetCookie("SID", cl.newSessionId, "/", true, srv.cookieSecure, (int)ttl, srv.cookieSameSite) + "\r\n";
        cl.newSessionId.clear();
    }
}

void Server::handleRequestException(Client &cl, const ServerConf &srv, const std::exception &e)
{

    Logger::error(std::string("Server exception in processRequest: ") + e.what());

    cl.resp = Responder::makeError(srv, 500);

    if (!cl.newSessionId.empty() && cl.resp.headers.find("Set-Cookie") == cl.resp.headers.end())
    {
        unsigned int ttl = srv.sessionTTL ? srv.sessionTTL : SessionManager::instance().getTTL();
        cl.resp.headers["Set-Cookie"] = Libft::makeSetCookie("SID", cl.newSessionId, "/", true, srv.cookieSecure, (int)ttl, srv.cookieSameSite);
        cl.newSessionId.clear();
    }
    cl.outBuf = cl.resp.serialize();
}
