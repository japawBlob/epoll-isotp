#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "ds.h"
#include "handle.h"

struct ep_entry * add_tcp_connection(struct epoll_instance * epoll, short int port)
{
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family      = AF_INET;              // IPv4
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);    // Bind to all available interfaces
    saddr.sin_port        = htons(port);          // Requested port
    if ( bind(sfd, (struct sockaddr *) &saddr, sizeof(saddr)) == -1 ){
        return NULL;
    }
    int flags = fcntl(sfd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(sfd, F_SETFL, flags);

    listen(sfd, SOMAXCONN);

    struct ep_entry *e;
    e = new_e();
    e->type = ESW_EPOLL_TCP_INIT;
    e->fd = sfd;
    add_e(epoll, e);
    return e;
}

int handle_tcp_connection(struct ep_entry *e, struct epoll_instance * epoll)
{
    int cfd;
    if ( (cfd = accept(e->fd, NULL, NULL)) == -1){
        return -1;
    }
    fcntl(cfd, F_SETFL, O_NONBLOCK);
    epoll->ep_set[epoll->ep_cnt++] = add_tcp_communication(epoll, cfd);
    return 0;
}

struct ep_entry * add_tcp_communication(struct epoll_instance * epoll, int fd)
{
    struct ep_entry *e;
    e = new_e();
    e->type = EWS_EPOLL_TCP_COMMUNICATION;
    e->fd = fd;
    e->count = 0;
    add_e(epoll, e);
    return e;
}

#define SMOL_BUFF_SIZE 42
int handle_tcp_communication(struct ep_entry *e)
{
    char temp_buffer [SMOL_BUFF_SIZE];
    memset(temp_buffer, 0, SMOL_BUFF_SIZE);
    int temp_count = read(e->fd, temp_buffer, SMOL_BUFF_SIZE-1);
    //temp_buffer[temp_count-1] = 'a';
    if(temp_count != -1 && temp_count != 0){
        memcpy(e->buffer+e->count, temp_buffer, temp_count);
        e->count += temp_count;
        char * terminator;
        while ( ( terminator = strstr (e->buffer/* + ( count - temp_count ) */, "\n") ) != NULL ) {
            *terminator = '\0';
            char ret [10];
            sprintf(ret, "%lu\n", strlen(e->buffer));
            write(e->fd, ret, strlen(ret));
            // usleep(100);
            e->count -= ( strlen(e->buffer) + 1 );
            memcpy(e->buffer, terminator+1, BUF_SIZE);
        }
    }
    return 0;
}
