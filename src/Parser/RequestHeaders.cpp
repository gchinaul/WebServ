#include "../include/Server.hpp"
#include "../include/LibftCpp.hpp"
#include "../include/Logger.hpp"
#include "../include/RequestParser.hpp"
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <string>
#include <cerrno>

void RequestParser::setHeader(const std::string &k, const std::string &v)
{
    std::string key = Libft::toLower(k);
    _req.headers[key] = v;

    if (key == "content-length")
        _req.contentLength = Libft::atoi(v.c_str());
    else if (key == "transfer-encoding" && v.find("chunked") != std::string::npos)
        _req.chunked = true;
}

bool RequestParser::parseHeaders(std::string &headerPart, int &errCode)
{

    std::istringstream iss(headerPart);
    std::string line;

    if (!std::getline(iss, line))
    {

        errCode = 400;
        return (false);
    }

    if (!line.empty() && line[line.size() - 1] == '\r')
        line = line.substr(0, line.size() - 1);

    if (!parseStartLine(line, errCode))
        return (false);

    while (std::getline(iss, line))
    {

        if (!line.empty() && line[line.size() - 1] == '\r')
            line = line.substr(0, line.size() - 1);

        if (line.empty())
            break;

        size_t colonPos = line.find(':');

        if (colonPos == std::string::npos)
        {

            errCode = 400;
            return (false);
        }

        std::string key = line.substr(0, colonPos);
        std::string value = line.substr(colonPos + 1);

        key = Libft::trim(key);
        value = Libft::trim(value);

        if (key.empty())
        {

            errCode = 400;
            return (false);
        }

        std::string keyLower = Libft::toLower(key);
        if (keyLower == "content-length")
        {
            for (size_t i = 0; i < value.size(); ++i)
            {
                if (!isdigit((unsigned char)value[i]))
                {
                    errCode = 400;
                    return (false);
                }
            }
        }

        setHeader(key, value);
    }

    if (_req.chunked && _req.contentLength > 0)
    {
        errCode = 400;
        return (false);
    }

    if (_req.version == "HTTP/1.1" && _req.headers.find("host") == _req.headers.end())
    {

        errCode = 400;
        return (false);
    }

    std::map<std::string, std::string>::const_iterator ctIt = _req.headers.find("content-type");

    if (ctIt != _req.headers.end())
    {

        if (ctIt->second.find("multipart/form-data") != std::string::npos)
        {

            _streamToFile = true;
            std::string tmpl = "/tmp/webserv_body_XXXXXX";

            int fd = ft_mkstemp(tmpl);

            if (fd != -1)
            {

                _streamFilePath = tmpl;
                _req.bodyFile = _streamFilePath;

                close(fd);

                _streamFile = new std::ofstream(_streamFilePath.c_str(), std::ios::out | std::ios::binary);

                if (!_streamFile || !_streamFile->good())
                {

                    if (_streamFile)
                    {

                        delete _streamFile;
                        _streamFile = NULL;
                    }

                    _streamToFile = false;
                    remove(_streamFilePath.c_str());
                }
            }
            else
                _streamToFile = false;
        }
    }

    return (true);
}

