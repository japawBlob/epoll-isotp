#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <unistd.h>

#include "ds.h"
#include "handle.h"

struct ep_entry * add_timer(struct epoll_instance * epoll, unsigned int ms)
{
	struct ep_entry *e;
	struct timespec t = {.tv_sec = ms / 1000,
		.tv_nsec = (ms % 1000) * 1000000};
	struct itimerspec it = {.it_interval = t, .it_value = t};
	e = new_e();
	e->type = ESW_EPOLL_TIMER;
	e->fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
	if (e->fd == -1) {
		perror("add_timer timerfd_create");
		return NULL;
	}
	if (timerfd_settime(e->fd, 0, &it, NULL) == -1) {
		perror("add_timer timerfd_settime");
		return NULL;
	}
	add_e(epoll, e);
	return e;
}

int handle_timer(struct ep_entry *e)
{
	uint64_t in;
	if (read(e->fd, &in, 8) == -1) {
		return -1;
	}
	printf("%d: %ld\n", e->fd, in);
	return 0;
}
