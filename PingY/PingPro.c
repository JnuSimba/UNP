
#include "ping.h"

#define MAX_SEND_TIME 5
	
void ping(void);
void sigint(int sig);
void *signal_set(int signo, void (*func)(int));
void signal_init();
struct sockaddr_in dest_addr;
char ip[20];

int main(int argc, char *argv[])
{
	// check parameter
	if(argc != 2)
	{
		printf("Usage: ./pingy IP\n");
		exit(1);
	}
	struct hostent *host;
	int netaddr;
	/*如果传递的是主机名 */
    if( (netaddr=inet_addr(argv[1])) == INADDR_NONE)
    {       
	if((host = gethostbyname(argv[1]))== NULL) /*获取ip*/ 
	{
	     perror("gethostbyname error");
             exit(1);
        }
        memcpy((char *)&dest_addr.sin_addr, host->h_addr, host->h_length);
     }
     else    /*传递的是ip*/
         dest_addr.sin_addr.s_addr = netaddr;
	

	ping();
	
	return 0;
}

void ping(void)
{
	int sockfd;
	int times = 0;

	if((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1)
	{
		printf("Create Socket Fail!\n");
		exit(1);
	}

	// fill dstination address
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(0);
	if(inet_pton(AF_INET, ip, &dest_addr.sin_addr) == -1)
	{
		printf("Get IP %s Error!\n", ip);
		exit(1);
	}

	// set signal
	signal_init();

	// print header
	print_head(ip);

	// ping
	while(times < MAX_SEND_TIME)
	{
		send_ping(sockfd, &dest_addr);
		recv_ping(sockfd, &dest_addr);
		times++;
	}

	// print statistics
	print_stat(ip);
}

/* ============================================================================ */
/*                                Singal Handle                                 */
/* ============================================================================ */
void sigint(int sig)
{
	print_stat(ip);
	exit(0);
}

void *signal_set(int signo, void (*func)(int))
{
	int ret;
	struct sigaction sig;
	struct sigaction osig;

	sig.sa_handler = func;
	sigemptyset(&sig.sa_mask);
	sig.sa_flags = 0;
#ifdef SA_RESTART
	sig.sa_flags |= SA_RESTART;
#endif /* SA_RESTART */

	ret = sigaction(signo, &sig, &osig);

	if (ret < 0) 
		return (SIG_ERR);
	else
		return (osig.sa_handler);
}

/* Initialization of signal handles. */
void signal_init()
{
	signal_set(SIGINT, sigint);
	signal_set(SIGTERM, sigint);
}
