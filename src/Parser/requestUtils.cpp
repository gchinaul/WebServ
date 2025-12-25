
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

bool RequestParser::feed(const char *data, size_t len, size_t maxBody, int &errCode)
{

    errCode = 0;
    _buf.append(data, len);

    if (!_headersDone)
    {

        size_t headerEnd = _buf.find("\r\n\r\n");

        if (headerEnd != std::string::npos)
        {

            std::string headerPart = _buf.substr(0, headerEnd);
            std::string rest = _buf.substr(headerEnd + 4);

            if (!parseHeaders(headerPart, errCode))
                return (false);

            _headersDone = true;
            _buf = rest;
        }
        else if (_buf.size() > 8192)
        {

            errCode = 414;
            return (false);
        }
        else
            return (false);
    }

    if (_headersDone && !_bodyDone)
        return parseBody(_buf, maxBody, errCode);

    return (_bodyDone);
}

bool RequestParser::done() const
{

    return (_bodyDone);
}

Request RequestParser::get() const
{

    return (_req);
}

bool RequestParser::decodeChunked(std::string &rest, int &errCode)
{

    while (true)
    {

        size_t crlfPos = rest.find("\r\n");

        if (crlfPos == std::string::npos)
            return (false);

        std::string chunkSizeLine = rest.substr(0, crlfPos);
        size_t chunkSize = Libft::strtol(chunkSizeLine.c_str(), NULL, 16);

        if (chunkSize == 0)
        {

            size_t endPos = rest.find("\r\n\r\n", crlfPos);

            if (endPos == std::string::npos)
                return (false);

            _bodyDone = true;

            return (true);
        }

        size_t dataStart = crlfPos + 2;
        size_t dataEnd = dataStart + chunkSize;

        if (rest.size() < dataEnd + 2)
            return (false);

        _req.body += rest.substr(dataStart, chunkSize);
        rest = rest.substr(dataEnd + 2);

        if (_req.body.size() > 1024 * 1024)
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
    }
}
