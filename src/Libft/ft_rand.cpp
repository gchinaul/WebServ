
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>

#include "../include/Webserv.hpp"

static unsigned long ft_rand_state = 1;

void Libft::ft_srand(ft_time_t seed)
{

    if (seed == 0)
        seed = 1;

    ft_rand_state = (unsigned long)seed;
}

int Libft::ft_rand()
{

    ft_rand_state = (1103515245 * ft_rand_state + 12345) & 0x7FFFFFFF;

    return (int)(ft_rand_state);
}
