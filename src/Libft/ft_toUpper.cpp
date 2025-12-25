
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>

#include "../include/Webserv.hpp"

char Libft::toUpper(const char ch)
{

    char upperchar = ch;

    if (ch >= 'a' && ch <= 'z')
        upperchar = ch - 'a' + 'A';

    return (upperchar);
}
