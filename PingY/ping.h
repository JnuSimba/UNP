

#ifndef PING_H
#define PING_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>

#define uint16_t unsigned short

/*
 * Function:	cal_cksum 
 * Purpose:		calculate checksum of icmp header
 * Parameters:	addr - start address of icmp header
 *				len - icmp length
 * Return:		calculated checksum
 */
uint16_t cal_cksum(uint16_t *addr, int len);

/*
 * Function:	print_head
 * Purpose:		print head info of ping
 * Parameters:	ip - ping destination ip address
 * Return:		none
 */
void print_head(char *ip);

/*
 * Function:	print_stat
 * Purpose:		print statistics of ping
 * Parameters:	ip - ping destination ip address
 * Return:		none
 */
void print_stat(char *ip);

/*
 * Function:	send_ping
 * Purpose:		send ping [v4]
 * Parameters:	sockfd - socket file descriptor to send
 *				dstaddr - dstination address
 * Return:		none
 */
void send_ping(int sockfd, struct sockaddr_in *dstaddr);

/*
 * Function:	recv_ping
 * Purpose:		receive ping [v4]
 * Parameters:	sockfd - socket file descriptor to receive
 *				dstaddr - dstination address
 * Return:		none
 */
void recv_ping(int sockfd, struct sockaddr_in *dstaddr);

#endif
