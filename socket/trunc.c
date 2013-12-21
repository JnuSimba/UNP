#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define ERR_EXIT(m) \
        do \
        { \
                perror(m); \
                exit(EXIT_FAILURE); \
        } while(0)

int main(void)
{
	int sock;
	if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
		ERR_EXIT("socket");

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(5188);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sock, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
		ERR_EXIT("bind");

	sendto(sock, "ABCD", 4, 0, (struct sockaddr*)&servaddr, sizeof(servaddr));
	
	char recvbuf[1];
	int n;
	int i;
	for (i=0; i<4; i++)
	{
		/* udp是报式协议，即若一次性接收的空间小于发来的数据，有可能造成报文截断，
		 * 但一定没有tcp的粘包问题  */
		n = recvfrom(sock, recvbuf, sizeof(recvbuf), 0, NULL, NULL);
		if (n == -1)
		{
			if (errno == EINTR)
				continue;
			ERR_EXIT("recvfrom");
		}
		else if(n > 0)
			printf("n=%d %c\n", n, recvbuf[0]);
	}
	return 0;
}

