#include <sys/resource.h>
#include <fcntl.h>
#include <fstream>
#include <limits.h>

#include "../include/Webserv.hpp"

void CGI::parseCgiHeaders(const std::string &raw, std::map<std::string, std::string> &h, std::string &body)
{

    size_t pos = raw.find("\r\n\r\n");
    size_t sepLen = 4;
    if (pos == std::string::npos)
    {
        pos = raw.find("\n\n");
        sepLen = 2;
    }
    if (pos == std::string::npos)
    {
        body = raw;
        return;
    }
    std::string head = raw.substr(0, pos);
    body = raw.substr(pos + sepLen);

    std::vector<std::string> lines;
    size_t i = 0, j;
    while (i < head.size())
    {
        j = head.find("\r\n", i);
        if (j == std::string::npos)
        {
            j = head.find('\n', i);
            if (j == std::string::npos)
            {
                lines.push_back(head.substr(i));
                break;
            }
            lines.push_back(head.substr(i, j - i));
            i = j + 1;
        }
        else
        {
            lines.push_back(head.substr(i, j - i));
            i = j + 2;
        }
    }

    for (size_t k = 0; k < lines.size(); ++k)
    {
        size_t c = lines[k].find(':');
        if (c != std::string::npos)
        {
            std::string kx = lines[k].substr(0, c);
            std::string vx = lines[k].substr(c + 1);
            h[Libft::trim(kx)] = Libft::trim(vx);
        }
    }
}
