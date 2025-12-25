#include <sys/resource.h>
#include <fcntl.h>
#include <fstream>
#include <limits.h>

#include "../include/Webserv.hpp"

Response CGI::toResponse(CGIProcess &p)
{

    Response r;

    r.status = 200;
    r.headers["Date"] = HTTP::dateNow();
    r.headers["Connection"] = "keep-alive";

    std::map<std::string, std::string> h;
    std::string body;

    parseCgiHeaders(p.outBuf, h, body);

    std::map<std::string, std::string>::iterator it;
    it = h.find("Status");
    if (it != h.end())
        r.status = atoi(it->second.c_str());
    it = h.find("Content-Type");
    if (it != h.end())
        r.headers["Content-Type"] = it->second;
    else
        r.headers["Content-Type"] = "text/plain";

    r.body = body;
    r.headers["Content-Length"] = Libft::itoa(r.body.size());

    return (r);
}
