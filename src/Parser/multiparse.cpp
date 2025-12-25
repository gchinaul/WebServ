
#include "MultipartParser.hpp"
#include "LibftCpp.hpp"

std::vector<MultipartPart> MultipartParser::parse(const std::string &body, const std::string &contentType)
{

    std::vector<MultipartPart> res;
    size_t bpos = contentType.find("boundary=");

    if (bpos == std::string::npos)
        return (res);

    std::string boundary = contentType.substr(bpos + 9);

    if (boundary.size() >= 2 && boundary[0] == '"' && boundary[boundary.size() - 1] == '"')
        boundary = boundary.substr(1, boundary.size() - 2);

    std::string delim = "--" + boundary;

    size_t pos = 0;
    pos = body.find(delim, pos);

    if (pos == std::string::npos)
        return res;
    pos += delim.size();

    if (pos + 2 <= body.size() && body.substr(pos, 2) == "\r\n")
        pos += 2;

    while (pos < body.size())
    {

        if (body.substr(pos, delim.size()) == delim && body.substr(pos, delim.size() + 2) == delim + "--")
            break;

        size_t headerEnd = body.find("\r\n\r\n", pos);

        if (headerEnd == std::string::npos)
            break;

        std::string headerBlock = body.substr(pos, headerEnd - pos);
        std::map<std::string, std::string> headers;
        std::vector<std::string> lines = Libft::split(headerBlock, '\n');

        for (size_t i = 0; i < lines.size(); ++i)
        {

            std::string line = lines[i];

            if (!line.empty() && line[line.size() - 1] == '\r')
                line = line.substr(0, line.size() - 1);
            size_t colon = line.find(':');

            if (colon == std::string::npos)
                continue;

            std::string k = Libft::toLower(Libft::trim(line.substr(0, colon)));
            std::string v = Libft::trim(line.substr(colon + 1));
            headers[k] = v;
        }

        size_t dataStart = headerEnd + 4;
        size_t next = body.find(delim, dataStart);

        if (next == std::string::npos)
            break;

        size_t dataEnd = next;

        if (dataEnd >= 2 && body.substr(dataEnd - 2, 2) == "\r\n")
            dataEnd -= 2;

        MultipartPart part;
        part.headers = headers;
        part.body = body.substr(dataStart, dataEnd - dataStart);
        std::map<std::string, std::string> cd;

        std::map<std::string, std::string>::iterator it = headers.find("content-disposition");
        if (it != headers.end())
        {

            parseContentDisposition(it->second, cd);
            std::map<std::string, std::string>::iterator nit = cd.find("name");
            if (nit != cd.end())
                part.name = nit->second;
            nit = cd.find("filename");
            if (nit != cd.end())
                part.filename = nit->second;
        }
        res.push_back(part);

        pos = next + delim.size();

        if (pos + 2 <= body.size() && body.substr(pos, 2) == "--")
            break;

        if (pos + 2 <= body.size() && body.substr(pos, 2) == "\r\n")
            pos += 2;
    }

    return (res);
}
