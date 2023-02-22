#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "ds.h"
#include "handle.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_BUFF 100

struct ep_entry * add_stdin(struct epoll_instance * epoll)
{
	struct ep_entry *e;
	e = new_e();
	e->type = ESW_EPOLL_STDIN;
	e->fd = STDIN_FILENO;
	add_e(epoll, e);
	return e;
}

int handle_stdin(struct ep_entry *e)
{
	char in[MAX_BUFF];
	memset(in, 0, MAX_BUFF);
	if (read(e->fd, &in, MAX_BUFF) == -1) {
		return -1;
	}
	if (write(STDOUT_FILENO, &in, MAX_BUFF) == -1) {
		return -1;
	}
	return 0;
}
