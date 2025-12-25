
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

bool RequestParser::parseStartLine(const std::string &line, int &errCode)
{

    std::istringstream iss(line);
    std::string version;

    if (!(iss >> _req.method >> _req.uri >> version))
    {

        errCode = 400;
        return (false);
    }

    for (size_t i = 0; i < _req.method.size(); ++i)
        _req.method[i] = (char)Libft::toUpper((unsigned char)_req.method[i]);

    if (version != "HTTP/1.1" && version != "HTTP/1.0")
    {

        errCode = 505;
        return (false);
    }

    _req.version = version;
    size_t queryPos = _req.uri.find('?');

    if (queryPos != std::string::npos)
    {

        _req.path = _req.uri.substr(0, queryPos);
        _req.query = _req.uri.substr(queryPos + 1);
    }
    else
    {

        _req.path = _req.uri;
        _req.query = "";
    }

    if (_req.method != "GET" && _req.method != "POST" && _req.method != "DELETE" && _req.method != "HEAD")
    {

        errCode = 405;
        return (false);
    }

    return (true);
}

bool RequestParser::parseBody(std::string &rest, size_t maxBody, int &errCode)
{

    if (_req.chunked)
        return (decodeChunked(rest, errCode));
    else if (_req.contentLength > 0)
    {

        if (_req.contentLength > maxBody)
        {

            errCode = 413;

            if (_streamFile)
            {
                _streamFile->close();
                delete _streamFile;
                _streamFile = NULL;
            }
            if (!_streamFilePath.empty())
            {
                remove(_streamFilePath.c_str());
                _streamFilePath.clear();
            }
            _streamToFile = false;

            return (false);
        }

        if (_streamToFile && _streamFile)
        {

            size_t toWrite = rest.size();

            if (_streamedBytes + toWrite > _req.contentLength)
                toWrite = _req.contentLength - _streamedBytes;

            _streamFile->write(rest.data(), toWrite);
            _streamedBytes += toWrite;
            rest = rest.substr(toWrite);

            if (_streamedBytes >= _req.contentLength)
            {

                _streamFile->close();
                delete _streamFile;
                _streamFile = NULL;
                _streamToFile = false;
                _bodyDone = true;
                return (true);
            }

            return (false);
        }
        else
        {

            if (rest.size() >= _req.contentLength)
            {

                _req.body = rest.substr(0, _req.contentLength);
                _bodyDone = true;
                return (true);
            }
            return (false);
        }
    }
    else
    {
if (_req.method == "POST" || _req.method == "PUT")
    {
        errCode = 411;
        return (false);
    }
    _bodyDone = true;
    return (true);
    }
}
