#ifndef NET_SERVER_H
#define NET_SERVER_H

#include <stdio.h>
#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <unistd.h>    // For close function
#include <pthread.h>   // For pthread_create
#include <stdlib.h>   // For malloc and free

#define handle_error(cmd,result) \
    if (result<0)    \
    {                \
        perror(cmd); \
        return -1;   \
    }


void net_server_init();


#endif 