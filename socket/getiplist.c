/*************************************************************************
	> File Name: echoser.c
	> Author: Simba
	> Mail: dameng34@163.com 
	> Created Time: Fri 01 Mar 2013 06:15:27 PM CST
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string.h>
#include<netdb.h>

#define ERR_EXIT(m) \
	do { \
		perror(m); \
		exit(EXIT_FAILURE); \
	} while (0)

int getlocalip(char *ip)
{
	char host[100] = {0};
	if (gethostname(host, sizeof(host)) < 0)
		return -1;

	struct hostent *hp;
	if ((hp = gethostbyname(host)) == NULL)
		return -1;
	//  #define h_addr h_addr_list[0]
	strcpy(ip, inet_ntoa(*(struct in_addr*)hp->h_addr_list[0]));
		
	return 0;
}

int main(void)
{
	char host[100] = {0};
	if (gethostname(host, sizeof(host)) < 0)
		ERR_EXIT("gethostname error");

	struct hostent *hp;
	if ((hp = gethostbyname(host)) == NULL)
		ERR_EXIT("gethostbyname error");

	int i = 0;
	while (hp->h_addr_list[i] != NULL) {
		
		printf("%s\n", inet_ntoa(*(struct in_addr*)hp->h_addr_list[i]));
		i++;
	}

	char ip[16] = {0};
	getlocalip(ip);
	printf("local ip : %s\n" , ip);
	return 0;
}

