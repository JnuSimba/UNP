#ifndef SESSION_H_
#define SESSION_H_
#include "common.h"

typedef struct session
{
	//控制连接
	uid_t uid;
	int ctrl_fd;
	char cmdline[MAX_COMMAND_LINE];
	char cmd[MAX_COMMAND];
	char args[MAX_ARG];

	// ftp协议进程与nobody进程通信
	int ftp_fd;
	int nobody_fd;

	//限速
	unsigned int upload_speed_max;
	unsigned int download_speed_max;
	long transfer_start_sec;
	long transfer_start_usec;

	//数据连接
	struct sockaddr_in* p_sock;
	int data_fd;
	int pasv_listen_fd;
	int data_process;

	//ftp协议控制
	int is_ascii;
	long long restart_pos;
	char* rnfr_name;
	int abor_received;

	//客户端连接数控制
	int num_clients;
	int num_this_ip;

} session_t;


void begin_session(session_t* sess);

#endif // session.h