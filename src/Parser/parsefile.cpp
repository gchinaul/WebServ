
#include "MultipartParser.hpp"
#include "LibftCpp.hpp"

std::vector<MultipartPart> MultipartParser::parseFile(const std::string &filePath, const std::string &contentType)
{

    bool ok = false;
    std::string body = Libft::readFile(filePath, &ok);
    if (!ok)
        return std::vector<MultipartPart>();

    return (parse(body, contentType));
}
