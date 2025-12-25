
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>

#include "../include/Webserv.hpp"

std::string Libft::htmlEscape(const std::string &str)
{

    std::string result;

    for (size_t i = 0; i < str.size(); ++i)
    {

        if (str[i] == '<')
            result += "&lt;";
        else if (str[i] == '>')
            result += "&gt;";
        else if (str[i] == '&')
            result += "&amp;";
        else if (str[i] == '"')
            result += "&quot;";
        else
            result += str[i];
    }

    return (result);
}
