/*************************************************************************
	> File Name: byteorder.c
	> Author: Simba
	> Mail: dameng34@163.com 
	> Created Time: Fri 01 Mar 2013 04:16:08 PM CST
 ************************************************************************/

#include<stdio.h>
#include<arpa/inet.h>

int main(void)
{
	unsigned int x = 0x12345678;
	unsigned char *p = (unsigned char*)&x;
	printf("%x %x %x %x\n", p[0], p[1], p[2], p[3]);

	unsigned int y = htonl(x);
	p = (unsigned char*)&y;
	printf("%x %x %x %x\n", p[0], p[1], p[2], p[3]);
	
	return 0;
}

