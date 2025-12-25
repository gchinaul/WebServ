
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

Response Responder::sendFile(const ServerConf &srv, const std::string &fsPath)
{
    Response r;
    r.status = 200;
    r.headers["Date"] = HTTP::dateNow();
    r.headers["Connection"] = "keep-alive";

    bool ok = false;

    std::string body = Libft::readFile(fsPath, &ok);

    if (!ok)
        return (makeError(srv, 404));

    r.headers["Content-Type"] = HTTP::guessContentType(fsPath);
    r.headers["Content-Length"] = Libft::itoa(body.size());
    r.body = body;

    return (r);
}
