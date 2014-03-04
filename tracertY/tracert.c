#include "tracert.h"

#define UDPPACKET_SIZE 12
/* 40字节的数据报包含20字节的IP首部，8字节的UDP首部和12字节的用户数据
  在《tcp/ip协议详解》中12字节数据包含每发一个数据报就加1的序号，送出TTL的副本以及发送数据报的时间）
 */

void send_tracert(int sndsock, struct sockaddr_in *dstaddr, int ttl)
{
	char buf[UDPPACKET_SIZE];
	memset(buf, 'a', sizeof(buf));	// using bzero() would be more efficient for small array
	int n;

	if(setsockopt(sndsock, IPPROTO_IP, IP_TTL, (char *)&ttl, sizeof(ttl)) == -1)
	{
		perror("Set TTL Fail!\n");
		return;
	}

	if((n = sendto(sndsock, buf, UDPPACKET_SIZE, 0, (struct sockaddr *)dstaddr, sizeof(struct sockaddr_in))) == -1)
	{
		printf("Send Packet Fail!\n");
		return;
	}

	if(n != UDPPACKET_SIZE)
	{
		printf("Send Packet Fail!\n");
		return;
	}

}

int recv_tracert(int rcvsock, struct sockaddr_in *srcaddr, char *databuf, int buflen)
{
	int n;
	int len = sizeof(struct sockaddr_in);
	
	struct timeval timeout = {3, 0};
    	setsockopt(rcvsock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));

	if((n = recvfrom(rcvsock, databuf, buflen, 0, (struct sockaddr *)srcaddr, &len)) == -1)
	{
		if (errno == EWOULDBLOCK)
			return -1;
	}

	return n;
}

/*
* Function:	check_packet
* Purpose:		check packet, whether icmp packet, and if time out packet, or if dst port unreachable packet
* Parameters:	databuf - packet data buf
*				buflen - packet data buf length
*/
int check_packet(u_char *databuf, int buflen)
{
	int hlen1,hlen2,icmplen,ret;
    	socklen_t len;
	struct ip *ip,*hip;
	struct icmp *icmp;
	struct udphdr *udp;

	ip = (struct ip*)databuf;
	hlen1 = ip->ip_hl << 2;
	icmp = (struct icmp*)(databuf+hlen1);
	if((icmplen = buflen - hlen1) < 8)
		return -1;
	
	if (icmp->icmp_type == ICMP_TIMXCEED &&
		icmp->icmp_code == ICMP_TIMXCEED_INTRANS)
	{
		if (icmplen < 8+sizeof(struct ip))
			return -1;
		// get icmp data
		hip = (struct ip*)(databuf + hlen1 + 8);
		hlen2 = hip->ip_hl << 2;
		if (icmplen < 8 + hlen2 +4)
			return -1;

		udp = (struct udphdr*)(databuf + hlen1 + 8 + hlen2);
		if (hip->ip_p == IPPROTO_UDP &&
			udp->source == htons(SRCPORT)
			&& udp->dest == htons(DSTPORT))
		{
			return -2;// timeout icmp
		}
	}
	else if(icmp->icmp_type == ICMP_UNREACH)
	{
		if (icmplen < 8 + sizeof(struct ip))
			return -1;
		hip = (struct ip*)(databuf + hlen1 + 8 + hlen2);
		if (hip->ip_p == IPPROTO_UDP
			&& udp->source == htons(SRCPORT)
			&& udp->dest == htons(DSTPORT))
		{
			return ret = icmp->icmp_code; // unreachable icmp
		}

	}
	else
		return -1;
}

/*
* Fuction:		subtimeval
* Purpose:		substract packet send time t1 and receive time t2, to get round trip time
* Parameters:	t1 - packet send time
*				t2 - packet receive time
* Return:		delta time(in ms)
*/
double subtimeval(struct timeval *t1, struct timeval *t2)
{
	int subsec = t2->tv_sec - t1->tv_sec;		// second = 1000 000microsecond
	int subusec = t2->tv_usec - t1->tv_usec;	// microsecond
	return subsec * 1000 + subusec / 1000.0;
}

/*
 * Function:	print_head
 * Purpose:		print traceroute program header information, including dst ip, max hops and 
 *				send bytes
 * Parameters:	dst - destination address
 * Return:		none
 */
void print_head(char *dst)
{
	int size = UDPPACKET_SIZE + sizeof(struct ip) + sizeof(struct udphdr);
	printf("traceroute to %s, %d hops max, %d byte packets\n", dst, MAX_TTL, size);
}

/*
 * Function:	inet_ntoay
 * Purpose:		just like inet_ntoa, using self-defined function only because system running error
 *				on my test computer
 * Parameters:	addr - address in byte format
 *				ip - transfered address in printable format
 * Return:		address of transfered address
 */
char *inet_ntoay(unsigned long addr, unsigned char *ip)
{
	unsigned char *p = (unsigned char *)&addr;
	sprintf(ip, "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
	return ip;
}
