#ifndef _SYS_UTIL_H_
#define _SYS_UTIL_H_

#include "common.h"

int tcp_client(unsigned int port);
int tcp_server(const char* host, unsigned short port);

int getlocalip(char *ip);
bool GetLocalIp(uint32_t *ip);

void activate_nonblock(int fd);
void deactivate_nonblock(int fd);

int read_timeout(int fd, unsigned int wait_seconds);
int write_timeout(int fd, unsigned int wait_seconds);
int accept_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds);
int connect_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds);

ssize_t readn(int fd, void *buf, size_t count);
ssize_t writen(int fd, const void *buf, size_t count);
ssize_t recv_peek(int sockfd, void *buf, size_t len);
ssize_t readline(int sockfd, void *buf, size_t maxline);

void send_fd(int sock_fd, int fd);
int recv_fd(const int sock_fd);
const char* statbuf_get_date(struct stat *sbuf);
const char* statbuf_get_perms(struct stat *sbuf);

int lock_read_file(int fd);
int lock_write_file(int fd);
int unlock_file(int fd);
long get_curtime_sec(void);
long get_curtime_usec(void);

void nano_sleep(double time_sleep);

void active_oobinline(int fd);
void active_sigurg(int fd);

#endif /* _SYS_UTIL_H_ */

