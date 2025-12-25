
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

std::string Response::serialize() const
{

    std::string out;

    out += "HTTP/1.1 " + Libft::itoa(status) + " " + HTTP::reason(status) + "\r\n";

    std::map<std::string, std::string>::const_iterator it;

    for (it = headers.begin(); it != headers.end(); ++it)
        out += it->first + ": " + it->second + "\r\n";

    out += "\r\n";
    out += body;

    return (out);
}
