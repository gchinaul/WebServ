
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

Response Responder::listDir(const ServerConf &srv, const std::string &urlPath, const std::string &fsPath)
{

    DIR *d = opendir(fsPath.c_str());

    if (!d)
        return (makeError(srv, 403));

    std::string html = "<html><body><h1>Index of " + Libft::htmlEscape(urlPath) + "</h1><ul>";

    struct dirent *ent;
    while ((ent = readdir(d)))
    {

        std::string name = ent->d_name;
        if (name == ".")
            continue;
        std::string href = (urlPath.size() && urlPath[urlPath.size() - 1] == '/') ? urlPath : urlPath + "/";
        html += "<li><a href=\"" + Libft::htmlEscape(href + name) + "\">" + Libft::htmlEscape(name) + "</a></li>";
    }

    closedir(d);

    html += "</ul></body></html>";

    Response r;
    r.status = 200;
    r.headers["Date"] = HTTP::dateNow();
    r.headers["Content-Type"] = "text/html; charset=UTF-8";
    r.headers["Content-Length"] = Libft::itoa(html.size());
    r.headers["Connection"] = "keep-alive";
    r.body = html;

    return (r);
}
