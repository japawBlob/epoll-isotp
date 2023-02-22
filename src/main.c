#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "handle.h"

int rv0 = 0;

static void term(int sn)
{
	if (sn == SIGINT || sn == SIGTERM) {
		rv0 = 1;
	}
}

int main()
{
	//int epfd, ep_cnt = 0, i;
	//struct ep_entry *ep_set[MAX_EVENTS];
    struct epoll_instance epoll;
    epoll.ep_cnt = 0;

	struct sigaction sa = {.sa_handler = term, .sa_flags = SA_RESTART};
	if (sigaction(SIGINT, &sa, NULL) == -1) {
		perror("sigaction");
		exit(EXIT_FAILURE);
	}
	if (sigaction(SIGTERM, &sa, NULL) == -1) {
		perror("sigaction");
		exit(EXIT_FAILURE);
	}
	epoll.fd = epoll_create1(0);
	if (epoll.fd == -1) {
		perror("epoll_create1");
		exit(EXIT_FAILURE);
	}
	epoll.ep_set[epoll.ep_cnt++] = add_stdin(&epoll);
    epoll.ep_set[epoll.ep_cnt++] = add_timer(&epoll, 1500);
    // epoll.ep_set[epoll.ep_cnt++] = add_timer(&epoll, 2000);
    epoll.ep_set[epoll.ep_cnt++] = add_tcp_connection(&epoll, 12345);
	for (int i = 0; i < epoll.ep_cnt; i++) {
		if (epoll.ep_set[i] == NULL) {
			perror("add_...");
			exit(EXIT_FAILURE);
		}
	}
	while (handle_all(&epoll) == rv0) {
	}
	for (int i = 0; i < epoll.ep_cnt; i++) {
		del_e(&epoll, epoll.ep_set[i]);
	}
	close(epoll.fd);
	return 0;
}
