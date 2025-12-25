
#include <dirent.h>
#include <cstdlib>
#include <cstdio>
#include <sys/stat.h>
#include <unistd.h>

#include "Responder.hpp"
#include "Config.hpp"
#include "Server.hpp"
#include "MultipartParser.hpp"
#include "HTTP.hpp"
#include "LibftCpp.hpp"
#include "Session.hpp"
#include "Logger.hpp"

static std::string errorPagePath(const ServerConf &srv, int code)
{

    return (Libft::pathJoin(srv.errorDir, Libft::itoa(code) + ".html"));
}

Response Responder::makeError(const ServerConf &srv, int code)
{

    Response r;
    r.status = code;
    r.headers["Date"] = HTTP::dateNow();
    r.headers["Connection"] = "keep-alive";

    bool ok = false;

    std::string body = Libft::readFile(errorPagePath(srv, code), &ok);

    if (!ok)
        body = "<html><body><h1>" + Libft::itoa(code) + " " + HTTP::reason(code) + "</h1></body></html>";

    r.headers["Content-Type"] = "text/html; charset=UTF-8";
    r.headers["Content-Length"] = Libft::itoa(body.size());
    r.body = body;

    return (r);
}
