
#include <unistd.h>

#include "Webserv.hpp"

static const char *CLR_RESET = "\033[0m";
static const char *CLR_INFO = "\033[0;32m";
static const char *CLR_ERROR = "\033[0;31m";
static const char *CLR_WARN = "\033[0;33m";
static const char *CLR_DEBUG = "\033[0;36m";

void Logger::info(const std::string &s)
{

    std::cout << CLR_INFO;
    std::cout << "[" << HTTP::dateNow() << "] [INFO] " << s << std::endl;
    std::cout << CLR_RESET;
}

void Logger::error(const std::string &s)
{

    std::cerr << CLR_ERROR;
    std::cerr << "[" << HTTP::dateNow() << "] [ERROR] " << s << std::endl;
    std::cerr << CLR_RESET;
}

void Logger::warning(const std::string &s)
{

    std::cerr << CLR_WARN;
    std::cerr << "[" << HTTP::dateNow() << "] [WARNING] " << s << std::endl;
    std::cerr << CLR_RESET;
}

void Logger::debug(const std::string &s)
{

    std::cout << CLR_DEBUG;
    std::cout << "[" << HTTP::dateNow() << "] [DEBUG] " << s << std::endl;
    std::cout << CLR_RESET;
}
