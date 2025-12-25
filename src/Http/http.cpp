
#include "../include/Webserv.hpp"

static const HTTPStatus kStatuses[] = {

    {200, "OK"}, {201, "Created"}, 
    {204, "No Content"}, {301, "Moved Permanently"}, 
    {302, "Found"}, {400, "Bad Request"}, {403, "Forbidden"}, 
    {404, "Not Found"}, {405, "Method Not Allowed"}, 
    {413, "Payload Too Large"}, {500, "Internal Server Error"}, 
    {501, "Not Implemented"}, {504, "Gateway Timeout"}, {0, 0}

};

const char *HTTP::reason(int code)
{

    for (int i = 0; kStatuses[i].reason; i++)
    {

        if (kStatuses[i].code == code)
            return (kStatuses[i].reason);
    }
    return ("");
}

std::string HTTP::guessContentType(const std::string &p)
{

    size_t dot = p.find_last_of('.');
    if (dot == std::string::npos)
        return ("application/octet-stream");

    std::string ext = p.substr(dot + 1);

    if (ext == "html" || ext == "htm")
        return ("text/html; charset=UTF-8");

    if (ext == "css")
        return ("text/css");

    if (ext == "js")
        return ("application/javascript");

    if (ext == "png")
        return ("image/png");

    if (ext == "jpg" || ext == "jpeg")
        return ("image/jpeg");

    if (ext == "gif")
        return ("image/gif");

    if (ext == "txt")
        return ("text/plain; charset=UTF-8");

    return ("application/octet-stream");
}
