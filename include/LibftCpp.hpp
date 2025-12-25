/*
 * Libft functions made with C++98
 *
 */

#pragma once
#ifndef LIBFTCPP_HPP
#define LIBFTCPP_HPP

#include "Webserv.hpp"

namespace Libft
{
    int             ft_rand();
    int             isspace(int c);
    int             hexToInt(char c);
    int             atoi(const char *s);
    int             atoi(const std::string& s);

    char            toUpper(const char ch);
    char            *xstrdup(const std::string& s);

    long            strtol(const char *nptr, char **endptr, int base);
    unsigned long   strtoul(const std::string& s);

    void            ft_srand(ft_time_t seed);
    void*           ft_memset(void* ptr, int value, size_t num);

    bool            isDir(const std::string& path);
    bool            fileExists(const std::string& path);
    bool            startsWith(const std::string& s, const std::string& p);
    bool            endsWith(const std::string& s, const std::string& suf);
    bool            writeFile(const std::string& path, const std::string& data);

    std::string     itoa(long n);
    std::string     trim(const std::string& s);
    std::string     toLower(const std::string& s);
    std::string     urlDecode(const std::string& str);
    std::string     htmlEscape(const std::string& str);
    std::string     ft_inet_ntoa(struct in_addr addr);
    std::string     readFile(const std::string& path, bool* ok);
    std::string     pathJoin(const std::string& a, const std::string& b);
    std::string     safePath(const std::string &root, const std::string& urlPath);
    std::string     join(const std::vector<std::string>& parts, const std::string& sep);
    std::string     makeSetCookie(const std::string& name, const std::string& value, const std::string& path, bool httpOnly, bool secure=false, int maxAge=0, const std::string &sameSite="");

    in_addr_t       ft_inet_addr(const char *host);

    std::vector<std::string> split(const std::string& str, char delim);

    std::map<std::string, std::string> parseCookies(const std::string& cookieHeader);

};

#endif
