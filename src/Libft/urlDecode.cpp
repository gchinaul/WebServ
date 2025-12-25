
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>

#include "../include/Webserv.hpp"

std::string Libft::urlDecode(const std::string &str)
{

    std::string result;

    for (size_t i = 0; i < str.size(); ++i)
    {

        if (str[i] == '%' && i + 2 < str.size())
        {

            char c = hexToInt(str[i + 1]) * 16 + hexToInt(str[i + 2]);
            result += c;
            i += 2;
        }
        else if (str[i] == '+')
            result += ' ';
        else
            result += str[i];
    }

    return (result);
}
