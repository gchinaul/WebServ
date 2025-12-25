
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>

#include "../include/Webserv.hpp"

long Libft::strtol(const char *nptr, char **endptr, int base)
{

    const char *s = nptr;

    while (*s && (*s == ' ' || *s == '\t' || *s == '\n' || *s == '\r' || *s == '\v' || *s == '\f'))
        ++s;

    int sign = 1;

    if (*s == '+' || *s == '-')
    {

        if (*s == '-')
            sign = -1;
        ++s;
    }

    if (base == 0)
    {

        if (*s == '0')
        {

            if (s[1] == 'x' || s[1] == 'X')
            {

                base = 16;
                s += 2;
            }
            else
            {

                base = 8;
                ++s;
            }
        }
        else
            base = 10;
    }
    else if (base == 16)
    {

        if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
            s += 2;
    }

    const char *start = s;
    unsigned long acc = 0;

    for (; *s; ++s)
    {

        unsigned char c = (unsigned char)*s;
        int digit;

        if (c >= '0' && c <= '9')
            digit = c - '0';
        else if (c >= 'a' && c <= 'z')
            digit = c - 'a' + 10;
        else if (c >= 'A' && c <= 'Z')
            digit = c - 'A' + 10;
        else
            break;

        if (digit >= base)
            break;

        acc = acc * base + (unsigned)digit;
    }

    if (endptr)
    {

        if (s == start)
            *endptr = const_cast<char *>(nptr);
        else
            *endptr = const_cast<char *>(s);
    }

    long result = (long)acc;

    if (sign < 0)
        result = -result;

    return (result);
}
