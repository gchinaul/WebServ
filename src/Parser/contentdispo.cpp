
#include "MultipartParser.hpp"
#include "LibftCpp.hpp"

void MultipartParser::parseContentDisposition(const std::string &val, std::map<std::string, std::string> &out)
{

    std::vector<std::string> parts = Libft::split(val, ';');

    for (size_t i = 0; i < parts.size(); ++i)
    {

        std::string p = Libft::trim(parts[i]);
        size_t eq = p.find('=');

        if (eq == std::string::npos)
        {

            std::string key = Libft::toLower(p);
            out[key] = "";
        }
        else
        {

            std::string k = Libft::toLower(Libft::trim(p.substr(0, eq)));
            std::string v = Libft::trim(p.substr(eq + 1));

            if (v.size() >= 2 && v[0] == '"' && v[v.size() - 1] == '"')
                v = v.substr(1, v.size() - 2);
            out[k] = v;
        }
    }
}
