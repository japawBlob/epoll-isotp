#define _GNU_SOURCE

#include <net/if.h>
#include <sys/socket.h>
#include <sys/types.h>
// #include <arpa/inet.h>
#include <fcntl.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <linux/can.h>
#include <linux/can/isotp.h>

#include "ds.h"
#include "handle.h"



#define SMOL_BUFF_SIZE 42

#define CAN_TX_MSG_ID 			0x70A
#define CAN_RX_MSG_ID 			0x774
#define CAN_TX_PADDING 			0x55
#define CAN_RX_PADDING 			0xAA
#define CAN_MSG_BUFSIZE 		5000
#define CAN_MAX_MSG_BYTES_CNT		30

#define BUFSIZE 67000 /* size > 66000 kernel buf to test socket API internal checks */


struct ep_entry * add_isotp_connection(struct epoll_instance * epoll)
{	
	printf("epoll created\n");
    int s;
    struct sockaddr_can addr;
    extern int optind, opterr, optopt;

	addr.can_addr.tp.tx_id = 0x123;
	addr.can_addr.tp.rx_id = 0x321;

    if ((s = socket(PF_CAN, SOCK_DGRAM, CAN_ISOTP)) < 0) {
		perror("socket");
		exit(1);
    }

    addr.can_family = AF_CAN;
    addr.can_ifindex = if_nametoindex("vcan0");
    if (!addr.can_ifindex) {
		perror("if_nametoindex");
		exit(1);
    }

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("bind");
		close(s);
		exit(1);
    }

	int flags;
	int ret;
	if (-1 == (flags = fcntl(s, F_GETFL, 0)))
		flags = 0;
	ret = fcntl(s, F_SETFL, flags | O_NONBLOCK);
	if (ret == -1) {
		perror("Make socket O_NONBLOCK");
		exit(1);
	}
	struct ep_entry *e;
    e = new_e();
    e->type = ESW_EPOLL_ISOTP_RECV;
    e->fd = s;
    add_e(epoll, e);
    return e;
}

int handle_isotp_recv(struct ep_entry *e, struct epoll_instance * epoll)
{	
	printf("message received\n");

	read(e->fd, e->buffer, BUF_SIZE);

	int s;
    struct sockaddr_can addr;
    extern int optind, opterr, optopt;

	addr.can_addr.tp.tx_id = 0x124;
	addr.can_addr.tp.rx_id = 0x322;

    if ((s = socket(PF_CAN, SOCK_DGRAM, CAN_ISOTP)) < 0) {
		perror("socket");
		exit(1);
    }

    addr.can_family = AF_CAN;
    addr.can_ifindex = if_nametoindex("vcan0");
    if (!addr.can_ifindex) {
		perror("if_nametoindex");
		exit(1);
    }

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("bind");
		close(s);
		exit(1);
    }

	int flags;
	int ret;
	if (-1 == (flags = fcntl(s, F_GETFL, 0)))
		flags = 0;
	ret = fcntl(s, F_SETFL, flags | O_NONBLOCK);
	if (ret == -1) {
		perror("Make socket O_NONBLOCK");
		exit(1);
	}

	

	struct ep_entry * write_entry1;
    write_entry1 = new_e();
	memcpy(write_entry1->buffer, e->buffer, BUF_SIZE);
    write_entry1->type = ESW_EPOLL_ISOTP_SEND;
    write_entry1->fd = s;

	write(s, e->buffer, BUF_SIZE);
    add_out(epoll, write_entry1);
    return 0;
}

int handle_isotp_send(struct ep_entry *e)
{
	// printf("message send\n");
	write(e->fd, e->buffer, BUF_SIZE);
    return 0;
}