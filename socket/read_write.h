#ifndef READ_WRITE_H_
#define READ_WRITE_H_

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<netdb.h>
#include<errno.h>

#ifdef __cplusplus
extern "C"
{
#endif

ssize_t readn(int, void*, size_t);
ssize_t writen(int, const void*, size_t);
ssize_t recv_peek(int, void*, size_t);
ssize_t readline(int, void*, size_t);

#ifdef __cplusplus
}
#endif

#endif
