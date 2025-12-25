
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>

#include "../include/Webserv.hpp"

void *Libft::ft_memset(void *ptr, int value, size_t num)
{

    if (!ptr)
        return (NULL);

    unsigned char *byte_ptr = static_cast<unsigned char *>(ptr);
    unsigned char byte_value = static_cast<unsigned char>(value);

    for (size_t i = 0; i < num; i++)
        byte_ptr[i] = byte_value;

    return (ptr);
}
