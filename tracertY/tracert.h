/*
 * Realize tracert command by myself
 *  - set TTL = 1, send ICMP ECHO packet, get time out packet
 *  - set TTL = 2, send ICMP ECHO packet, get time out packet
 *  - ...until get ICMP ECHO REPLY packet
 * ============================================================================ 
 */

#ifndef _TRACERT_H
#define _TRACERT_H

#include <netinet/in.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#define SRCPORT		5005	// UDP packet source port

#define DSTPORT		58127	// UDP packet destination port
#define MAX_TTL	255	// Max hop

/*
 * Function:	send_tracert
 * Purpose:		send icmp packet using UDP with self-defined TTL
 * Parameters:	sndsock - socket to send
 *				dstaddr - destination address
 *				ttl - TTL
 * Return:		none
 */
void send_tracert(int sndsock, struct sockaddr_in *dstaddr, int ttl);

/*
 * Function:	recv_tracert
 * Purpose:		receive icmp packet using origin socket
 * Parameters:	rcvsock - socket to receive
 *				srcaddr - address of packet from
 *				databuf - packet data
 *				buflen - packet data length
 * Return:		receive bytes count
 */
int recv_tracert(int rcvsock, struct sockaddr_in *srcaddr, char *databuf, int buflen);

/*
 * Function:	check_packet
 * Purpose:		check packet, whether icmp packet, and if time out packet, or if dst port unreachable packet
 * Parameters:	databuf - packet data buf
 *				buflen - packet data buf length
 * Return:		0 if not expected packet, 1 if expected packet
 */
int check_packet(u_char *databuf, int buflen);

/*
 * Fuction:		subtimeval
 * Purpose:		substract packet send time t1 and receive time t2, to get round trip time
 * Parameters:	t1 - packet send time
 *				t2 - packet receive time
 * Return:		delta time(in ms)
 */
double subtimeval(struct timeval *t1, struct timeval *t2);

/*
 * Function:	print_head
 * Purpose:		print traceroute program header information, including dst ip, max hops and 
 *				send bytes
 * Parameters:	dst - destination address
 * Return:		none
 */
void print_head(char *dst);

/*
 * Function:	inet_ntoay
 * Purpose:		just like inet_ntoa, using self-defined function only because system running error
 *				on my test computer
 * Parameters:	addr - address in byte format
 *				ip - transfered address in printable format
 * Return:		address of transfered address
 */
char *inet_ntoay(unsigned long addr, unsigned char *ip);

#endif
