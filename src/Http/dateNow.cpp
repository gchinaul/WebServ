
#include "Webserv.hpp"

std::string HTTP::dateNow()
{
    char buf[128];

    ft_time_t t = ft_time();
    struct ft_tm *gmt = ft_gmtime(&t);

    const char *days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    int len = 0;

    for (int i = 0; days[gmt->tm_wday][i]; i++)
        buf[len++] = days[gmt->tm_wday][i];
    buf[len++] = ',';
    buf[len++] = ' ';

    buf[len++] = '0' + (gmt->tm_mday / 10);
    buf[len++] = '0' + (gmt->tm_mday % 10);
    buf[len++] = ' ';

    for (int i = 0; months[gmt->tm_mon][i]; i++)
        buf[len++] = months[gmt->tm_mon][i];
    buf[len++] = ' ';

    int year = 1900 + gmt->tm_year;
    buf[len++] = '0' + (year / 1000);
    buf[len++] = '0' + ((year / 100) % 10);
    buf[len++] = '0' + ((year / 10) % 10);
    buf[len++] = '0' + (year % 10);
    buf[len++] = ' ';

    buf[len++] = '0' + (gmt->tm_hour / 10);
    buf[len++] = '0' + (gmt->tm_hour % 10);
    buf[len++] = ':';

    buf[len++] = '0' + (gmt->tm_min / 10);
    buf[len++] = '0' + (gmt->tm_min % 10);
    buf[len++] = ':';

    buf[len++] = '0' + (gmt->tm_sec / 10);
    buf[len++] = '0' + (gmt->tm_sec % 10);
    buf[len++] = ' ';

    buf[len++] = 'G';
    buf[len++] = 'M';
    buf[len++] = 'T';
    buf[len++] = '\0';

    return std::string(buf);
}
