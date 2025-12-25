
#pragma once
#ifndef TIME_HPP
# define TIME_HPP

#include "../include/Webserv.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string>

typedef long ft_time_t;

struct ft_tm {

    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};

inline ft_time_t ft_time()
{
    struct stat st;

    int fd = open("/tmp/.time_temp", O_CREAT | O_WRONLY | O_TRUNC, 0644);

    if (fd != -1)
    {
        int ret = write(fd, "x", 1);
        (void)ret;
        close(fd);

        if (stat("/tmp/.time_temp", &st) == 0)
        {
            return (ft_time_t)st.st_mtime;
        }
    }

    return 0;
}

inline struct ft_tm* ft_gmtime(const ft_time_t* timer)
{
    static struct ft_tm result;
    ft_time_t t = *timer;

    long days = t / 86400;
    long rm = t % 86400;

    result.tm_hour = rm / 3600;
    rm %= 3600;
    result.tm_min = rm / 60;
    result.tm_sec = rm % 60;

    result.tm_wday = (days + 4) % 7;

    long year = 1970;
    long day_of_year = days;

    while (1)
    {
        long days_in_year = 365;
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
            days_in_year = 366;
        
        if (day_of_year >= days_in_year)
        {
            day_of_year -= days_in_year;
            year++;
        }
        else
        {
            break;
        }
    }

    result.tm_year = year - 1900;
    result.tm_yday = day_of_year;

    static const int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int is_leap = ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));

    int month = 0;
    
    while (month < 12) {
        int dim = days_in_month[month];
        if (month == 1 && is_leap) dim = 29;

        if (day_of_year >= dim)
        {
            day_of_year -= dim;
            month++;
        }
        else
        {
            break;
        }
    }

    result.tm_mon = month;
    result.tm_mday = day_of_year + 1;
    result.tm_isdst = 0;

    return &result;
}

#endif