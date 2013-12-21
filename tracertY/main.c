/*
 * Realize tracert command by myself
 *  - set TTL = 1, send ICMP packet, get time out packet
 *  - set TTL = 2, send ICMP packet, get time out packet
 *  - ...until get ICMP Unreachable packet
 * ============================================================================ 
*/

#include "tracert.h"

#define MAXPACKET	65535	// Max size of IP packet

int main(int argc, char *argv[])
{
	struct sockaddr_in haddr;	// remote host address
	struct sockaddr_in laddr;	// local host address
	int sndsock, rcvsock;

	if(argc != 2)
	{
		printf("Usage: tracerty host\n");
		return 0;
	}

	// fill destination address
	bzero(&haddr, sizeof(haddr));	// why using bzero(), but not memset()
	haddr.sin_family = AF_INET;
	haddr.sin_port = htons(DSTPORT);

	struct hostent *host;
	int netaddr;
	/*判断是主机名还是ip地址*/
    if( (netaddr=inet_addr(argv[1])) == INADDR_NONE)
    {       
		if((host = gethostbyname(argv[1]))== NULL) /*是主机名*/
        {
			 perror("gethostbyname error");
             exit(1);
        }
        memcpy((char *)&haddr.sin_addr, host->h_addr, host->h_length);
     }
     else    /*是ip地址*/
        haddr.sin_addr.s_addr = netaddr;
	

	// fill local host address
	bzero(&laddr, sizeof(laddr));
	laddr.sin_family = AF_INET;
	laddr.sin_port = htons(SRCPORT);
	laddr.sin_addr.s_addr = htonl(INADDR_ANY);

	// create send socket
	if((sndsock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("Create Send UDP Socket Fail!\n");
		return 0;
	}

	int on = 1;
    if (setsockopt(sndsock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
        perror("setsockopt error");

	if(bind(sndsock, (struct sockaddr *)&laddr, sizeof(laddr)) == -1)
	{
		perror("Bind UDP Socket Fail!\n");
		return 0;
	}

	// create receive socket
	if((rcvsock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
	{
		printf("Create Receive RAW Socket Fail!\n");
		return 0;
	}

	// start to trace route
	print_head(argv[1]);

	int ttl, n;
	int get_des = 0;	// whether get destination
	int unreach = 0; // whether unreachable
	unsigned char ip[16] = {0};
	for(ttl = 1; ttl < MAX_TTL; ttl++)
	{
		int probe_count;
		int has_addr = 0;	// whether have printed address
		
		memset(ip ,0,sizeof(ip));

		for(probe_count = 0; probe_count < 3; probe_count++)
		{
			struct sockaddr_in from;
			struct timeval t1, t2;
			char buf[MAXPACKET];
			

			gettimeofday(&t1, 0);
			send_tracert(sndsock, &haddr, ttl);
			while((n = recv_tracert(rcvsock, &from, buf, MAXPACKET)) > 0)
			{
				gettimeofday(&t2, 0);
				int code = 255;
				if((code = check_packet(buf, sizeof(buf))) != -1)
				{
					// reach destination
					if (code == 3)
						get_des = 1;
					else if (code < 16 && code >= 0)
						unreach = 1;

					// print information
					if(has_addr == 0)
					{
						fprintf(stdout,  " %.2d", ttl);
						has_addr = 1;
					}
					
					fprintf(stdout, "  %.5fms  ", subtimeval(&t1, &t2));
					
					if (strlen(ip) == 0)
					{
						inet_ntoay(from.sin_addr.s_addr, ip);
					}
					break;
				}
				else
					continue;
			}

			if(n == -1)
			{
				if (has_addr == 0)
				{
					fprintf(stdout, " %.2d", ttl);
					has_addr = 1; 
				}
				printf("\t*\t");	 
			}

		}

		if (strlen(ip) != 0)
			printf("%s\n", ip);
		else
			printf("request timeout!\n");
		
	
		if(get_des == 1 || unreach == 1)
			break;
	}
	
	if (get_des == 1)
		printf("Get Destination, traceroute end.\n");
	else if (unreach == 1)
		printf("Destination unreachable, traceroute end.\n");
	else
		printf("Reach hop-max, traceroute end.\n");

	return 0;
}
