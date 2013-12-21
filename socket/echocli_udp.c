/*************************************************************************
	> File Name: echoser_udp.c
	> Author: Simba
	> Mail: dameng34@163.com 
	> Created Time: Sun 03 Mar 2013 06:13:55 PM CST
 ************************************************************************/

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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

void echo_cli(int sock)
{
	struct sockaddr_in servaddr;
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(5188);
        servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	/* 并没有真正建立一个连接，即没有3次握手过程
	 * 只是维护了一种状态，绑定了远程地址，*/
	connect(sock, (struct sockaddr*)&servaddr, sizeof(servaddr));

	int ret;
	char sendbuf[1024] = {0};
	char recvbuf[1024] = {0};
	while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL)
	{	
		/* connect没有绑定客户端的端口，而sendto时会绑定，如果客户端有多个ip，调用这两个函数
		 * 的参数（远程地址）会决定从哪个ip出去  */
		/* sendto只是把Buf的数据拷贝到sock对应的缓冲区中，若服务器未开启，协议栈返回一个ICMP异步错误
		 * 如果前面调用了connect“建立”了一个连接，则recvfrom时能收到这个错误，否则收不到而一直阻塞*/
		/*sendto(sock, sendbuf, strlen(sendbuf), 0, (struct sockaddr*)&servaddr, sizeof(servaddr));*/
		/*sendto(sock, sendbuf, strlen(sendbuf), 0, NULL, 0);*/
		
		send(sock, sendbuf, strlen(sendbuf), 0);
	
		ret = recvfrom(sock, recvbuf, sizeof(recvbuf), 0, NULL, NULL);
		if (ret == -1)
		{
			if (errno == EINTR)
				continue;
			ERR_EXIT("recvfrom");
		}

		fputs(recvbuf, stdout);
		memset(sendbuf, 0, sizeof(sendbuf));
		memset(recvbuf, 0, sizeof(recvbuf));
	}

	close(sock);


}

int main(void)
{
	int sock;
	if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
		ERR_EXIT("socket");

	echo_cli(sock);

	return 0;
}



