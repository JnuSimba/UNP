#ifndef SESSION_H_
#define SESSION_H_
#include "common.h"

typedef struct session
{
	//��������
	uid_t uid;
	int ctrl_fd;
	char cmdline[MAX_COMMAND_LINE];
	char cmd[MAX_COMMAND];
	char args[MAX_ARG];

	// ftpЭ�������nobody����ͨ��
	int ftp_fd;
	int nobody_fd;

	//����
	unsigned int upload_speed_max;
	unsigned int download_speed_max;
	long transfer_start_sec;
	long transfer_start_usec;

	//��������
	struct sockaddr_in* p_sock;
	int data_fd;
	int pasv_listen_fd;
	int data_process;

	//ftpЭ�����
	int is_ascii;
	long long restart_pos;
	char* rnfr_name;
	int abor_received;

	//�ͻ�������������
	int num_clients;
	int num_this_ip;

} session_t;


void begin_session(session_t* sess);

#endif // session.h