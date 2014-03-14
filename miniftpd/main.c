#include "common.h"
#include "sysutil.h"
#include "session.h"
#include "strtools.h"
#include "parseconf.h"
#include "tunable.h"
#include "ftpproto.h"
#include "ftpcodes.h"
#include "hash_link.h"


extern session_t* p_sess;
static unsigned int cur_childrens;

void handle_sigchld(int);
void check_clients_limit(session_t* sess);

static hash_t *s_ip_count_hash;
static hash_t *s_pid_ip_hash;
unsigned int hash_func(unsigned int buckets, void *key);
unsigned int handle_ip_count(void *ip);
void drop_ip_count(void* ip);

int main(void)
{

	daemon(1, 0);

	parseconf_load_file(MINIFTPD_CONF);

	if (getuid() != 0)
	{
		fprintf(stderr, "must be started by root.\n");
		exit(EXIT_FAILURE);
	}


	int listenfd = tcp_server(tunable_listen_address, tunable_listen_port);
	int conn;
	session_t sess =
	{
		//控制连接
		0, -1, "", "", "",

		// ftp协议进程与nobody进程通信
		-1, -1,
		
		//限速
		0, 0, 0, 0,
		// 数据连接
		NULL, -1, -1, 0,

		// ftp协议控制
		0, 0, NULL, 0,

		//客户端连接数控制
		0, 0
	};

	p_sess = &sess;
	sess.upload_speed_max = tunable_upload_max_rate;
	sess.download_speed_max = tunable_download_max_rate;

	signal(SIGCHLD, handle_sigchld);

	struct sockaddr_in peeraddr;
	
	s_ip_count_hash = hash_alloc(256, hash_func);
	s_pid_ip_hash = hash_alloc(256, hash_func);

	while(1)
	{
		if ((conn = accept_timeout(listenfd, &peeraddr, 0)) < 0)
			ERR_EXIT("accept_timeout");
		
		unsigned int ip = (unsigned int)peeraddr.sin_addr.s_addr;
	
		sess.num_this_ip = handle_ip_count(&ip);

		++cur_childrens;
		sess.num_clients = cur_childrens;
		

		pid_t pid = fork();
		if (pid == -1)
		{
			--cur_childrens;
			ERR_EXIT("fork");
		}

		if (pid == 0)
		{
			sess.ctrl_fd = conn;
			close(listenfd);
			check_clients_limit(&sess);
			signal(SIGCHLD, SIG_IGN);
			begin_session(&sess);
		}
		else if (pid > 0)
		{
			hash_add_entry(s_pid_ip_hash, &pid, sizeof(pid), &ip, sizeof(unsigned int));
			close(conn);
		}
	}

	hash_free(s_ip_count_hash);
	hash_free(s_pid_ip_hash);

	return 0;
}

void handle_sigchld(int sig)
{
	pid_t pid;
	// 如果多个子进程同时断开连接
	while ((pid = waitpid(-1 ,NULL, WNOHANG)) > 0)
	{
		unsigned int* ip = hash_lookup_entry(s_pid_ip_hash, &pid, sizeof(pid));
		if (ip == NULL)
		{
			continue;
		}

		drop_ip_count(ip);
		--cur_childrens;
		hash_free_entry(s_pid_ip_hash, &pid, sizeof(pid));
	}
	
}

void check_clients_limit(session_t* sess)
{
	if (tunable_max_clients > 0 && sess->num_clients > tunable_max_clients)
	{
		ftp_reply(sess, FTP_TOO_MANY_USERS, "There are too many connected users, Please try later.");
		exit(EXIT_FAILURE);
	}

	if (tunable_max_per_ip > 0 && sess->num_this_ip > tunable_max_per_ip)
	{
		ftp_reply(sess, FTP_TOO_MANY_USERS, 
			"There are too many connections from your internet address, Please try later.");
		exit(EXIT_FAILURE);
	}
}


unsigned int hash_func(unsigned int buckets, void *key)
{
	unsigned int* ip = (unsigned int*)key;
	return (*ip) % buckets;
}

unsigned int handle_ip_count(void *ip)
{
	unsigned int count;
	unsigned int* ip_count;
	ip_count = hash_lookup_entry(s_ip_count_hash, ip, sizeof(unsigned int));

	if (ip_count == NULL)
	{
		count = 1;
		hash_add_entry(s_ip_count_hash, ip, sizeof(unsigned int), &count, sizeof(unsigned int));
	}
	else
	{
		count = *ip_count;
		count++;
		*ip_count = count;
	}

	return count;
}

void drop_ip_count(void* ip)
{
	unsigned int count;
	unsigned int* ip_count;
	ip_count = hash_lookup_entry(s_ip_count_hash, ip, sizeof(unsigned int));

	if (ip_count == NULL)
	{
		return;
	}

	count = *ip_count;
	count--;

	if (count == 0)
	{
		hash_free_entry(s_ip_count_hash, ip, sizeof(unsigned int));
	}

	*ip_count = count;
}


