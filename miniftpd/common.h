#ifndef _COMMON_H_
#define _COMMON_H_

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <time.h>
#include <pwd.h>
#include <crypt.h>
#include <shadow.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pwd.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <ifaddrs.h>
#include <net/if.h> 
#include <netinet/in.h> 
#include <net/if_arp.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <linux/capability.h>
#include <sys/syscall.h>
#include <sys/sendfile.h>

#define ERR_EXIT(m) \
  do \
  { \
    perror(m); \
	exit(EXIT_FAILURE); \
  } \
  while (0)


#define MAX_COMMAND_LINE 1024
#define MAX_COMMAND 32
#define MAX_ARG 1024
#define MINIFTPD_CONF "miniftpd.conf"

#endif /* _COMMON_H_ */
