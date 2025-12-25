
#include <fstream>
#include <algorithm>
#include <signal.h>

#include "../include/Webserv.hpp"
#include "../include/Session.hpp"

void Server::processClientIO(const std::vector<int> &clientFds, const fd_set &readfds, const fd_set &writefds)
{

    for (size_t i = 0; i < clientFds.size(); ++i)
    {
        int fd = clientFds[i];
        if (_clients.find(fd) == _clients.end())
            continue;
        if (fd < 0)
        {
            Logger::error("Invalid FD in clientFds: " + Libft::itoa(fd));
            continue;
        }
        if (FD_ISSET(fd, &readfds))
            handleReadable(fd);
        if (_clients.find(fd) != _clients.end() && FD_ISSET(fd, &writefds))
            handleWritable(fd);

        if (_clients.find(fd) != _clients.end())
            processClientCGIIO(_clients[fd], readfds, writefds);
    }
}

void Server::processClientCGIIO(Client &cl, const fd_set &readfds, const fd_set &writefds)
{

    if (cl.usingCGI && cl.cgi.outFd != -1 && FD_ISSET(cl.cgi.outFd, &readfds))
    {

        char buf[4096];
        ssize_t n = read(cl.cgi.outFd, buf, sizeof(buf));

        if (n > 0)
        {
            cl.cgi.outBuf.append(buf, n);
        }
        else if (n == 0)
        {

            close(cl.cgi.outFd);
            cl.cgi.outFd = -1;

            try
            {
                Logger::debug("CGI raw output size=" + Libft::itoa((long)cl.cgi.outBuf.size()));

                if (!cl.cgi.outBuf.empty())
                {
                    std::string preview = cl.cgi.outBuf.substr(0, std::min((size_t)1024, cl.cgi.outBuf.size()));
                    Logger::debug("CGI raw output preview:\n" + preview);
                }
                cl.resp = CGI::toResponse(cl.cgi);
                const ServerConf &tsrv = *cl.srv;
                if (!cl.newSessionId.empty() && cl.resp.headers.find("Set-Cookie") == cl.resp.headers.end())
                {
                    unsigned int ttl = tsrv.sessionTTL ? tsrv.sessionTTL : SessionManager::instance().getTTL();
                    cl.resp.headers["Set-Cookie"] = Libft::makeSetCookie("SID", cl.newSessionId, "/", true, tsrv.cookieSecure, (int)ttl, tsrv.cookieSameSite);
                    cl.newSessionId.clear();
                }
                cl.outBuf = cl.resp.serialize();
            }
            catch (...)
            {
                Logger::error("Exception while converting CGI output to Response");
                cl.resp = Responder::makeError(*cl.srv, 500);
                const ServerConf &tsrv = *cl.srv;
                if (!cl.newSessionId.empty() && cl.resp.headers.find("Set-Cookie") == cl.resp.headers.end())
                {
                    unsigned int ttl = tsrv.sessionTTL ? tsrv.sessionTTL : SessionManager::instance().getTTL();
                    cl.resp.headers["Set-Cookie"] = Libft::makeSetCookie("SID", cl.newSessionId, "/", true, tsrv.cookieSecure, (int)ttl, tsrv.cookieSameSite);
                    cl.newSessionId.clear();
                }
            }
            cl.outBuf = cl.resp.serialize();

            if (cl.cgi.inFd != -1)
            {
                close(cl.cgi.inFd);
                cl.cgi.inFd = -1;
            }
            if (cl.cgi.errFd != -1)
            {
                close(cl.cgi.errFd);
                cl.cgi.errFd = -1;
            }
            if (cl.cgi.pid > 0)
            {
                int status = 0;
                waitpid(cl.cgi.pid, &status, WNOHANG);
                cl.cgi.pid = -1;
            }
            cl.usingCGI = false;
        }
    }

    if (cl.usingCGI && cl.cgi.errFd != -1 && FD_ISSET(cl.cgi.errFd, &readfds))
    {
        int errnum = 0;
        ssize_t r = read(cl.cgi.errFd, &errnum, sizeof(errnum));
        if (r == sizeof(errnum))
        {
            Logger::error("CGI exec failure, errno=" + Libft::itoa(errnum));
            cl.resp = Responder::makeError(*cl.srv, 500);
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
            close(cl.cgi.errFd);
            cl.cgi.errFd = -1;
            cl.usingCGI = false;
        }
    }

    if (cl.usingCGI && cl.cgi.inFd != -1 && FD_ISSET(cl.cgi.inFd, &writefds))
    {
        if (!cl.cgi.inBuf.empty())
        {
            ssize_t wm = write(cl.cgi.inFd, cl.cgi.inBuf.data(), cl.cgi.inBuf.size());
            if (wm > 0)
            {
                cl.cgi.inBuf.erase(0, (size_t)wm);
            }
            else if (wm < 0)
            {
                Logger::error("write() error to CGI stdin");
                close(cl.cgi.inFd);
                cl.cgi.inFd = -1;
            }
        }
        if (cl.cgi.inBuf.empty() && cl.cgi.inFd != -1)
        {
            close(cl.cgi.inFd);
            cl.cgi.inFd = -1;
        }
    }
}
