/*************************************************************************
	> File Name: addr_in.c
	> Author: Simba
	> Mail: dameng34@163.com 
	> Created Time: Fri 01 Mar 2013 04:16:08 PM CST
 ************************************************************************/

#include<stdio.h>
#include<arpa/inet.h>

int main(void)
{

	unsigned int  addr = inet_addr("192.168.0.100"); //转换后是网络字节序（大端）
	printf("add=%u\n", ntohl(addr));

	struct in_addr ipaddr;
	ipaddr.s_addr = addr;
	printf("%s\n", inet_ntoa(ipaddr));

	return 0;
}

