#define _GNU_SOURCE

#include <net/if.h>
#include <sys/socket.h>
#include <sys/types.h>
// #include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "ds.h"
#include "handle.h"
#include <linux/can.h>
#include "isotp.h"



#define SMOL_BUFF_SIZE 42

#define CAN_TX_MSG_ID 			0x70A
#define CAN_RX_MSG_ID 			0x774
#define CAN_TX_PADDING 			0x55
#define CAN_RX_PADDING 			0xAA
#define CAN_MSG_BUFSIZE 		5000
#define CAN_MAX_MSG_BYTES_CNT		30

struct ep_entry * add_isotp_connection(struct epoll_instance * epoll, short int port)
{
    const char * interface = "vcan0";
    struct sockaddr_can addr;
    struct ifreq ifr;
	static struct can_isotp_options opts;
	static struct can_isotp_fc_options fcopts;
	static struct can_isotp_ll_options llopts;
	__u32 force_rx_stmin = 0;
	extern int optind, opterr, optopt;
	int s;


	addr.can_addr.tp.tx_id = CAN_TX_MSG_ID;
	addr.can_addr.tp.rx_id = CAN_RX_MSG_ID;
	opts.txpad_content = CAN_TX_PADDING;
	opts.rxpad_content = CAN_RX_PADDING;
	opts.flags |= (CAN_ISOTP_TX_PADDING | CAN_ISOTP_RX_PADDING);

	if ((s = socket(PF_CAN, SOCK_DGRAM, CAN_ISOTP)) < 0) {
		perror("socket");
		return NULL;
	}

	CHECK(setsockopt(s, SOL_CAN_ISOTP, CAN_ISOTP_OPTS, &opts, sizeof(opts)));
	CHECK(setsockopt(s, SOL_CAN_ISOTP, CAN_ISOTP_RECV_FC, &fcopts, sizeof(fcopts)));

	if (llopts.tx_dl) {
		if (setsockopt(s, SOL_CAN_ISOTP, CAN_ISOTP_LL_OPTS, &llopts, sizeof(llopts)) < 0) {
			perror("link layer sockopt");
			return NULL;
		}
	}

	if (opts.flags & CAN_ISOTP_FORCE_RXSTMIN)
		setsockopt(s, SOL_CAN_ISOTP, CAN_ISOTP_RX_STMIN, &force_rx_stmin, sizeof(force_rx_stmin));

	addr.can_family = AF_CAN;
	strcpy(ifr.ifr_name, interface);
	ioctl(s, SIOCGIFINDEX, &ifr);
	addr.can_ifindex = ifr.ifr_ifindex;

	int flags;
	int ret;
	if (-1 == (flags = fcntl(s, F_GETFL, 0)))
		flags = 0;
	ret = fcntl(s, F_SETFL, flags | O_NONBLOCK);
	if (ret == -1) {
		perror("Make socket O_NONBLOCK");
		return -1;
	}
}

int handle_isotp_connection(struct ep_entry *e, struct epoll_instance * epoll)
{
    int cfd;
    if ( (cfd = accept(e->fd, NULL, NULL)) == -1){
        return -1;
    }
    fcntl(cfd, F_SETFL, O_NONBLOCK);
    epoll->ep_set[epoll->ep_cnt++] = add_tcp_communication(epoll, cfd);
    return 0;
}

struct ep_entry * add_isotp_communication(struct epoll_instance * epoll, int fd)
{
    struct ep_entry *e;
    e = new_e();
    e->type = EWS_EPOLL_TCP_COMMUNICATION;
    e->fd = fd;
    e->count = 0;
    add_e(epoll, e);
    return e;
}

int handle_isotp_server(struct ep_entry *e)
{
    char temp_buffer [SMOL_BUFF_SIZE];
    memset(temp_buffer, 0, SMOL_BUFF_SIZE);
    int temp_count = read(e->fd, temp_buffer, SMOL_BUFF_SIZE-1);
    if(temp_count != -1 && temp_count != 0){
        memcpy(e->buffer+e->count, temp_buffer, temp_count);
        e->count += temp_count;
        char * terminator;
        while ( ( terminator = strstr (e->buffer, "\n") ) != NULL ) {
            *terminator = '\0';
            char ret [10];
            sprintf(ret, "%lu\n", strlen(e->buffer));
            write(e->fd, ret, strlen(ret));
            e->count -= ( strlen(e->buffer) + 1 );
            memcpy(e->buffer, terminator+1, BUF_SIZE);
        }
    }
    return 0;
}

// int handle_isotp_recv(struct ep_entry *e)
// {
//     int blob = CAN_SFF_ID_BITS;
//     return 0;
// }

// int handle_isotp_send(struct ep_entry *e)
// {
//     return 0;
// }