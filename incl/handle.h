/* Process functions for ESW Epoll example. */
#ifndef ESW_HANDLE_H
#define ESW_HANDLE_H

#include "ds.h"

struct ep_entry *add_stdin(struct epoll_instance * epoll);
int handle_stdin(struct ep_entry *e);

struct ep_entry *add_timer(struct epoll_instance * epoll, unsigned int ms);
int handle_timer(struct ep_entry *e);

struct ep_entry * add_tcp_connection(struct epoll_instance * epoll, short int port);
int handle_tcp_connection(struct ep_entry *e, struct epoll_instance * epoll);

struct ep_entry * add_tcp_communication(struct epoll_instance * epoll, int fd);
int handle_tcp_communication(struct ep_entry *e);

struct ep_entry *new_e();
int add_e(struct epoll_instance * epoll, struct ep_entry *e);
int del_e(struct epoll_instance * epoll, struct ep_entry *e);
int handle_all(struct epoll_instance * epoll);

#endif /* ESW_HANDLE_H */
