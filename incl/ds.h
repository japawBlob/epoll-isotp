/* Data structures for ESW Epoll example. */
#ifndef ESW_DS_H
#define ESW_DS_H

#define MAX_EVENTS 64

#define BUF_SIZE 1024

struct epoll_instance {
    int fd, ep_cnt;
    struct ep_entry *ep_set[MAX_EVENTS];
};

struct ep_entry {
	int fd;
	enum {ESW_EPOLL_STDIN, ESW_EPOLL_TIMER, ESW_EPOLL_TCP_INIT, EWS_EPOLL_TCP_COMMUNICATION, ESW_EPOLL_ISOTP} type;
    int count;
    char buffer[BUF_SIZE];
};

#endif /* ESW_DS_H */
