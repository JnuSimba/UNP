#include <stdio.h>
#include <stdlib.h>
#include <netinet/ip_icmp.h>
#include <string.h>
#include "ping.h"

#define ICMP_ECHOREPLY 0
#define ICMP_ECHOREQ 8

// statistics
int nsend = 0, nrecv = 0;
int rrt[10];
int packsize;

void get_data(int rrt[], int size, int *min, int *max, int *avg);
void tv_sub(struct timeval *out,struct timeval *in);

// calculate checksum of icmp header
uint16_t cal_cksum(uint16_t *addr, int len)
{
	int nleft = len;
	uint32_t sum = 0;
	uint16_t *w = addr;
	uint16_t answer = 0;

	while (nleft > 1) {
		sum += *w++;
		nleft -= 2;
	}

	if (nleft == 1) {
		*(unsigned char *)(&answer) = *(unsigned char *)w ;
		sum += answer;
	}

	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	answer = ~sum;
	return(answer);
}

void print_head(char *ip)
{
	packsize = sizeof(struct icmp) + sizeof(struct timeval);
	printf("Ping %s with %d bytes of data:\n", ip, packsize);
}

void print_stat(char *ip)
{
	int min_rrt, max_rrt, avg_rrt;
	int lost;
	
	if(nsend < 1)
		return;
	get_data(rrt, nsend, &min_rrt, &max_rrt, &avg_rrt);

	lost = nsend - nrecv;
	printf("========================================================================\n");
	printf("Ping Statistics for %s:\n", ip);
	printf("\tPackets: Send = %d, Received = %d, Lost = %d(%.1f%% lost),\n", nsend, nrecv, lost, (lost / (nsend * 1.0) * 100.0));
	printf("Approximate round trip times in milli-seconds:\n");
	printf("\tMinimum = %dms, Maximum = %dms, Average = %dms\n", min_rrt, max_rrt, avg_rrt);
}

void send_ping(int sockfd, struct sockaddr_in *dstaddr)
{
	char buf[100];
	size_t len = sizeof(struct icmp);
	socklen_t dstlen = sizeof(struct sockaddr_in);
	struct icmp *echo;

	memset(buf, 0, sizeof(buf));
	echo = (struct icmp*)buf;
	echo = (struct icmp *)buf;
	echo->icmp_type = ICMP_ECHOREQ;
	echo->icmp_code = 0;
	echo->icmp_cksum = 0;
	echo->icmp_id = getpid();
	echo->icmp_seq = nsend;
	struct timeval *tval= (struct timeval *)echo->icmp_data;
	gettimeofday(tval,NULL);
	echo->icmp_cksum = cal_cksum((uint16_t *)echo, packsize);

	// send ping message
	if(sendto(sockfd, buf, len, 0, (struct sockaddr*)dstaddr, dstlen) == -1)
		printf("Send Ping Message Error!\n");
	
	nsend++;
}

void recv_ping(int sockfd, struct sockaddr_in *dstaddr)
{
	char buf[100];
	ssize_t n;
	struct ip *ip;
	struct icmp *icmp;
	socklen_t dstlen = sizeof(struct sockaddr_in);
	int ttl;
	fd_set rset;
	int maxfd = sockfd + 1;
	struct timeval timeo, *tvsend, tvrecv;
	unsigned char *p;
	unsigned char ipaddr[100];
	int time;

	memset(buf, 0, sizeof(buf));
	timeo.tv_sec = 3;
	timeo.tv_usec = 0;
	
	FD_ZERO(&rset);
	FD_SET(sockfd, &rset);
	
	
	while(1)
	{
		// set timeout 3s
		if((n = select(maxfd, &rset, NULL, NULL, &timeo)) == -1)
		{
			printf("Select Error!\n");
			exit(1);
		}

		if(n <= 0)
		{
			printf("Request Time Out!\n");
			fflush(stdout);
			break;
		}
		
		
		if((n = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)dstaddr, &dstlen)) == -1)
			printf("Recv Ping Message Error!\n");

		gettimeofday(&tvrecv, NULL);
		// check if icmp
		ip = (struct ip*)buf;
		ttl = ip->ip_ttl;

		// get src ip
		p = (unsigned char *)&ip->ip_src;
		sprintf(ipaddr, "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);

		// check if loop
		if(strcmp(ipaddr, "127.0.0.1") == 0)
		{
			//perror("Loop!");
			continue;
		}

		// check if icmp packet
		if(ip->ip_p != IPPROTO_ICMP)
		{
			//perror("Not ICMP Protocol!");
			continue;
		}

		// check if icmp reply
		icmp = (struct icmp*)(buf + sizeof(struct ip));
		if(icmp->icmp_type == ICMP_ECHOREPLY)
		{
			// check id
			if(icmp->icmp_id == getpid())
			{
				tvsend = (struct timeval *)icmp->icmp_data;
				tv_sub(&tvrecv, tvsend);
				time = tvrecv.tv_sec * 1000 + tvrecv.tv_usec / 1000;
				nrecv++;

				printf("\tReply from %s: bytes = %d time = %dms TTL = %d\n", ipaddr, n, time, ttl);
				rrt[nsend - 1] = time;
				break;
			}
			else
			{
				//perror("Not Expected Identifier!");
				continue;
			}
		}
		else
		{
			//perror("Not ICMP Reply Message!");
			continue;
		}
	}
}

/* ================================================================ */
/*                          Auxiliary Function                      */
/* ================================================================ */
/*
 * Function:	get_data
 * Purpose:		get statistics from data rrt array, such as minimum rrt,
 *				maximum rrt, average rrt
 * Parameters:	rrt - round trip time
 *				size - array size
 *				min - store minimum time
 *				max - store maximum time
 *				avg - store average time
 * Return:		none
 */
void get_data(int rrt[], int size, int *min, int *max, int *avg)
{
	int sum = 0;
	int i;

	*min = rrt[0], *max = rrt[0];

	for(i = 0; i < size; i++)
	{
		sum += rrt[i];
		if(rrt[i] < *min)
			*min = rrt[i];
		if(rrt[i] > *max)
			*max = rrt[i];
	}
	*avg = sum / size;
}

void tv_sub(struct timeval *out,struct timeval *in)
{       
	if((out->tv_usec -= in->tv_usec) < 0)
	{       
		--out->tv_sec;
		out->tv_usec += 1000000;
	}
	out->tv_sec -= in->tv_sec;
}
