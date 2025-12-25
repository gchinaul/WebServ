
#include <fstream>
#include <algorithm>
#include <signal.h>

#include "../include/Webserv.hpp"
#include "../include/Session.hpp"

void Server::checkCGITimeouts()
{

    time_t now_final = ft_time();
    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        Client &cl = it->second;
        if (cl.usingCGI && cl.cgi.pid > 0 && cl.cgi.startTime != 0 &&
            now_final - cl.cgi.startTime > (time_t)cl.srv->cgiTimeout)
        {

            Logger::warning("CGI timeout, killing pid " + Libft::itoa(cl.cgi.pid));
            kill(cl.cgi.pid, SIGKILL);
            waitpid(cl.cgi.pid, NULL, WNOHANG);
            cl.resp = Responder::makeError(*cl.srv, 504);

            const ServerConf &tsrv = *cl.srv;
            if (!cl.newSessionId.empty() && cl.resp.headers.find("Set-Cookie") == cl.resp.headers.end())
            {
                unsigned int ttl = tsrv.sessionTTL ? tsrv.sessionTTL : SessionManager::instance().getTTL();
                cl.resp.headers["Set-Cookie"] = Libft::makeSetCookie("SID", cl.newSessionId, "/", true, tsrv.cookieSecure, (int)ttl, tsrv.cookieSameSite);
                cl.newSessionId.clear();
            }

            cl.outBuf = cl.resp.serialize();

            if (cl.cgi.inFd != -1)
            {
                close(cl.cgi.inFd);
                cl.cgi.inFd = -1;
            }
            if (cl.cgi.outFd != -1)
            {
                close(cl.cgi.outFd);
                cl.cgi.outFd = -1;
            }
            if (cl.cgi.errFd != -1)
            {
                close(cl.cgi.errFd);
                cl.cgi.errFd = -1;
            }

            cl.usingCGI = false;
        }
    }
}
