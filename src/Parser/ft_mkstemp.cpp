
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

int RequestParser::ft_mkstemp(std::string &tmpl)
{
    static const char letters[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789";

    const int lettersCount = (int)(sizeof(letters) - 1);

    std::string::size_type pos = tmpl.find("XXXXXX");
    if (pos == std::string::npos)
        return -1;

    for (int attempt = 0; attempt < 100; ++attempt)
    {
        for (int i = 0; i < 6; ++i)
        {
            int r = Libft::ft_rand();
            if (r < 0)
                r = -r;
            tmpl[pos + i] = letters[r % lettersCount];
        }

        int fd = open(tmpl.c_str(), O_CREAT | O_EXCL | O_RDWR, 0600);
        if (fd >= 0)
        {
            return fd;
        }
        else if (errno != EEXIST)
        {
            return -1;
        }
    }

    return (-1);
}
