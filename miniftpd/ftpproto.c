
#include "ftpproto.h"
#include "sysutil.h"
#include "strtools.h"
#include "ftpcodes.h"
#include "common.h"
#include "tunable.h"
#include "privsock.h"

static void do_user(session_t *sess);
static void do_pass(session_t *sess);
static void do_cwd(session_t *sess);
static void do_cdup(session_t *sess);
static void do_quit(session_t *sess);
static void do_port(session_t *sess);
static void do_pasv(session_t *sess);
static void do_type(session_t *sess);
static void do_retr(session_t *sess);
static void do_stor(session_t *sess);
static void do_appe(session_t *sess);
static void do_list(session_t *sess);
static void do_nlst(session_t *sess);
static void do_rest(session_t *sess);
static void do_abor(session_t *sess);
static void do_pwd(session_t *sess);
static void do_mkd(session_t *sess);
static void do_rmd(session_t *sess);
static void do_dele(session_t *sess);
static void do_rnfr(session_t *sess);
static void do_rnto(session_t *sess);
static void do_site(session_t *sess);
static void do_syst(session_t *sess);
static void do_feat(session_t *sess);
static void do_size(session_t *sess);
static void do_stat(session_t *sess);
static void do_noop(session_t *sess);
static void do_help(session_t *sess);

static ftpcmd_t ctrl_cmds[] = {
	/* 访问控制命令 */
	{"USER",	do_user	},
	{"PASS",	do_pass	},
	{"CWD",		do_cwd	},
	{"XCWD",	do_cwd	},
	{"CDUP",	do_cdup	},
	{"XCUP",	do_cdup	},
	{"QUIT",	do_quit	},
	{"ACCT",	NULL	},
	{"SMNT",	NULL	},
	{"REIN",	NULL	},

	/* 传输参数命令 */
	{"PORT",	do_port	},
	{"PASV",	do_pasv	},
	{"TYPE",	do_type	},
	{"STRU",	NULL	},
	{"MODE",	NULL	},

	/* 服务命令 */
	{"RETR",	do_retr	},
	{"STOR",	do_stor	},
	{"APPE",	do_appe	},
	{"LIST",	do_list	},
	{"NLST",	do_nlst	},
	{"REST",	do_rest	},
	{"ABOR",	do_abor	},
	{"\377\364\377\362ABOR", do_abor},
	{"PWD",		do_pwd	},
	{"XPWD",	do_pwd	},
	{"MKD",		do_mkd	},
	{"XMKD",	do_mkd	},
	{"RMD",		do_rmd	},
	{"XRMD",	do_rmd	},
	{"DELE",	do_dele	},
	{"RNFR",	do_rnfr	},
	{"RNTO",	do_rnto	},
	{"SITE",	do_site	},
	{"SYST",	do_syst	},
	{"FEAT",	do_feat },
	{"SIZE",	do_size	},
	{"STAT",	do_stat	},
	{"NOOP",	do_noop	},
	{"HELP",	do_help	},
	{"STOU",	NULL	},
	{"ALLO",	NULL	},
	{ NULL,		NULL	}
};

session_t* p_sess;

void handle_alarm(int sig)
{
	shutdown(p_sess->ctrl_fd, SHUT_RD);
	ftp_reply(p_sess, FTP_IDLE_TIMEOUT, "Timeout.");
	shutdown(p_sess->ctrl_fd, SHUT_WR);
	exit(EXIT_FAILURE);
}

void start_cmd_alarm(void)
{
	if (tunable_idle_session_timeout > 0)
	{
		//安装信号
		signal(SIGALRM, handle_alarm);
		//开启闹钟
		alarm(tunable_idle_session_timeout);
	}
}

void handle_data_alarm(int sig);

void start_data_alarm(void)
{
	if (tunable_data_connection_timeout > 0)
	{
		//安装信号 
		signal(SIGALRM, handle_data_alarm);
		//开启闹钟 (如果此前有安装过闹钟，会取消之前的）
		alarm(tunable_data_connection_timeout);
	}
	else if (tunable_idle_session_timeout > 0)
	{
		alarm(0); //取消控制通道的闹钟
	}
}

void handle_data_alarm(int sig)
{
	if (!p_sess->data_process)
	{
		ftp_reply(p_sess, FTP_DATA_TIMEOUT, "Data timeout. Reconnect. Sorry.");
		exit(EXIT_FAILURE);
	}
	
	//如果此时正处在数据传输过程中
	start_data_alarm(); //重新开启数据通道闹钟
}

void handle_sigurg(int sig)
{
	if (p_sess->data_fd == -1)
	{
		return;
	}

	char cmdline[MAX_COMMAND_LINE] ={0};
	int ret = readline(p_sess->ctrl_fd, cmdline, MAX_COMMAND_LINE);
	if (ret <= 0)
	{
		ERR_EXIT("readline");
	}
	
	str_trim_crlf(cmdline);
	if (strcmp(cmdline, "ABOR") == 0 || 
		strcmp(cmdline, "\377\364\377\362ABOR") == 0)
	{
		p_sess->abor_received = 1;
		shutdown(p_sess->data_fd, SHUT_RDWR);
		p_sess->data_fd = -1;
	}
	else
		ftp_reply(p_sess, FTP_BADCMD, "Unknown command.");
}

void check_abor(session_t* sess)
{
	if (sess->abor_received)
	{
		ftp_reply(p_sess, FTP_ABOROK, "ABOR successful.");
	}
	sess->abor_received = 0;
}

void list_common(session_t* sess, int detail)
{
	DIR* dir = opendir(".");
	if (dir == NULL)
	{
		return;
	}

	struct dirent* ent;
	struct stat sbuf;
	
	while ((ent = readdir(dir)) != NULL)
	{
		if (lstat(ent->d_name, &sbuf) < 0)
			continue;

		if (strncmp(ent->d_name, ".", 1) == 0)
            continue; //忽略隐藏文件
	
		const char* perms = statbuf_get_perms(&sbuf);
		
		char buf[1024] = {0};

		if (detail)
		{
			int off = 0;
			off += sprintf(buf, "%s ", perms);
			off += sprintf(buf + off, " %3d %-8d %-8d ", sbuf.st_nlink, sbuf.st_uid, sbuf.st_gid);
			off += sprintf(buf + off, "%8lu ", (unsigned long)sbuf.st_size);

			const char *datebuf = statbuf_get_date(&sbuf);

			off += sprintf(buf + off, "%s ", datebuf);
			if (S_ISLNK(sbuf.st_mode))
			{
				char tmp[1024] = {0};
				readlink(ent->d_name, tmp, sizeof(tmp));
				off += sprintf(buf + off, "%s -> %s\r\n", ent->d_name, tmp);
			}
			else
			{
				off += sprintf(buf + off, "%s\r\n", ent->d_name);
			}
		}
		else
			sprintf(buf, "%s\r\n", ent->d_name);
		
		writen(sess->data_fd, buf, strlen(buf));
	}

	closedir(dir);
}

void limit_rate(session_t* sess, unsigned int transferd_bytes, int up_load)
{
	//传输速度 = 传输字节数 / 传输时间;

//IF 当前传输速度 > 最大传输速度 THEN
//	睡眠时间 = (当前传输速度 / 最大传输速度 – 1) * 当前传输时间;
	
	sess->data_process = 1; //正处于数据传输过程中

	long tsec = get_curtime_sec();
	long tusec = get_curtime_usec();
	double elapsed;
	elapsed = (double)(tsec - sess->transfer_start_sec);
	elapsed += (double)(tusec - sess->transfer_start_usec) / (double)1000000;
	
	if (elapsed <= (double)0.0)
	{
		elapsed = (double)0.01;
	}

	unsigned int transfer_speed = (unsigned int)((double)transferd_bytes / elapsed);
	unsigned int rate_ratio;

	if (up_load)
	{
		if (transfer_speed <=  sess->upload_speed_max || sess->upload_speed_max == 0 )
		{
			sess->transfer_start_sec = get_curtime_sec();
			sess->transfer_start_usec = get_curtime_usec();
			return; //不需要限速
		}

		rate_ratio = transfer_speed / sess->upload_speed_max;
	}

	else
	{
		if (transfer_speed <=  sess->download_speed_max || sess->download_speed_max == 0)
		{
			sess->transfer_start_sec = get_curtime_sec();
			sess->transfer_start_usec = get_curtime_usec();
			return; //不需要限速
		}

		rate_ratio = transfer_speed / sess->download_speed_max;
	}

	double time_sleep = ((double)rate_ratio - (double)1) * elapsed;

	nano_sleep(time_sleep);

	sess->transfer_start_sec = get_curtime_sec();
	sess->transfer_start_usec = get_curtime_usec();

}

void upload_common(session_t* sess, int is_append)
{
	//创建数据连接（每次数据连接都是临时的，数据传输完毕就关闭套接字）
	if (get_transfer_fd(sess) == 0)
	{
		//425 响应
		ftp_reply(sess, FTP_BADSENDCONN, "Use PORT or PASV first.");
		return;
	}

	long long offset = sess->restart_pos;
	sess->restart_pos = 0;


	//打开文件
	int fd = open(sess->args, O_CREAT | O_WRONLY, 0666);
	if (fd == -1)
	{
		ftp_reply(sess, FTP_UPLOADFAIL, "Could not create file.");
		return;
	}


	//文件加写锁
	int ret = lock_write_file(fd);
	if (ret == -1)
	{
		ftp_reply(sess, FTP_UPLOADFAIL, "Lock local file fail.");
		return;
	}

	if (!is_append && offset == 0) //STOR
	{
		ftruncate(fd, 0); //清空文件内容
		lseek(fd, 0, SEEK_SET); //定位到文件开头
	}

	else if (!is_append && offset != 0) //REST STOR
	{
		lseek(fd, offset, SEEK_SET); //定位到文件偏移位置
	}

	else if (is_append) //APPE
	{
		lseek(fd, 0, SEEK_END);//定位到文件末尾
	}
	
	ftp_reply(sess, FTP_DATACONN, "Ok to send data.");

	
	//读取数据套接字，写入本地文件
	
	int flag = 0;
	char buf[1024] = {0};


	sess->transfer_start_sec = get_curtime_sec();
	sess->transfer_start_usec = get_curtime_usec();

	while (1)
	{
		ret = read(sess->data_fd, buf, sizeof(buf));
		if (ret == -1)
		{
			if (errno == EINTR)
			{
				continue;
			}
			else
			{
				flag = 2;
				break;
			}
		}
		else if (ret == 0)
		{
			flag = 0;
			break;
		}

		limit_rate(sess, ret, 1);
		if (sess->abor_received)
		{
			flag = 2;
			break;
		}

		if (writen(fd, buf, ret) != ret)
		{
			flag = 1;
			break;
		}
	}

	//传输结束，关闭数据连接
	if (!sess->abor_received)
	{
		close(sess->data_fd);
		sess->data_fd = -1;
	}
	
	unlock_file(fd);
	close(fd);

	if (flag == 0 && !sess->abor_received)
	{
		//226 应答
		ftp_reply(sess, FTP_TRANSFEROK, "Transfer complete.");
	}

	if (flag == 1)
	{
		// 451响应
		ftp_reply(sess, FTP_BADSENDFILE, "Fail to write to local file.");
	}

	else if (flag == 2)
	{
		// 426 响应
		ftp_reply(sess, FTP_BADSENDNET, "Fail to read from network stream.");
	}

	check_abor(sess);
	sess->data_process = 0;
	start_cmd_alarm(); //数据传输完毕，重启启动控制通道闹钟
}

static void do_retr(session_t *sess)
{
//	 RETR /home/simba/Documents/tmp/miniftpd/main.c

	//创建数据连接（每次数据连接都是临时的，数据传输完毕就关闭套接字）
	if (get_transfer_fd(sess) == 0)
	{
		//425 响应
		ftp_reply(sess, FTP_BADSENDCONN, "Use PORT or PASV first.");
		return;
	}

	long long offset = sess->restart_pos;
	sess->restart_pos = 0;


	//打开文件
	int fd = open(sess->args, O_RDONLY);
	if (fd == -1)
	{
		ftp_reply(sess, FTP_FILEFAIL, "Open local file fail.");
		return;
	}


	//文件加读锁
	int ret = lock_read_file(fd);
	if (ret == -1)
	{
		ftp_reply(sess, FTP_FILEFAIL, "Lock local file fail.");
		return;
	}

	//判断文件传输模式（实际上应该还要配置 ascii_upload_enable 和 ascii_download_enable 两个变量）
	// 如果是ascii模式，则需要对\n转换成\r\n进行传输，在这边统一使用二进制方式传输，只是应答不同。
	
	char text[1024] = {0};
	struct stat sbuf;
	fstat(fd, &sbuf);
	
	//判断是否是普通文件（设备文件不能下载）
	if (!S_ISREG(sbuf.st_mode))
	{
		ftp_reply(sess, FTP_FILEFAIL, "It is not a regular file.");
		return;
	}

	if (!sess->is_ascii)
	{
		//150 响应
		sprintf(text, "Opening BINARY mode data connection for %s (%lld bytes).", 
			sess->args, (long long)sbuf.st_size);	

	}

	else
	{
		//150 响应
		sprintf(text, "Opening ASCII mode data connection for %s (%lld bytes).", 
			sess->args, (long long)sbuf.st_size);
	}

	ftp_reply(sess, FTP_DATACONN, text);

	//读取文件内容，写入数据套接字
	//  #include <sys/sendfile.h>
     //  sendfile()不涉及内核空间和用户空间的数据拷贝，效率比较高

	 // 如果是断点续载，定位到文件的断点处
	if (offset != 0)
	{
		ret = lseek(fd, offset, SEEK_SET);
		if (ret == -1)
		{
			ftp_reply(sess, FTP_FILEFAIL, "Lseek local file fail.");
			return;
		}
	}
	
	int flag = 0;
	long long bytes_tosend = sbuf.st_size;
	if (offset != 0)
	{
		bytes_tosend -= offset;
	}

	sess->transfer_start_sec = get_curtime_sec();
	sess->transfer_start_usec = get_curtime_usec();

	while (bytes_tosend > 0)
	{
		int thistime_tosend = bytes_tosend > 4096 ? 4096: bytes_tosend;
		ret = sendfile(sess->data_fd, fd, NULL, thistime_tosend);
		if (ret == -1)
		{
			flag = 2;
			break;
		}
		
		limit_rate(sess, ret, 0); //限速
		if (sess->abor_received)
		{
			flag = 2;
			break;
		}

		bytes_tosend -= ret;
	}

	//传输结束，关闭数据连接
	if (!sess->abor_received)
	{
		close(sess->data_fd);
		sess->data_fd = -1;
	}
	
	unlock_file(fd);
	close(fd);

	if (flag == 0 && !sess->abor_received)
	{
		//226 应答
		ftp_reply(sess, FTP_TRANSFEROK, "Transfer complete.");
		
	}

	else if (flag == 2)
	{
		// 426 响应
		ftp_reply(sess, FTP_BADSENDNET, "Fail to write to network stream.");
	
	}
	
	check_abor(sess);	
	sess->data_process = 0;
	start_cmd_alarm(); //数据传输完毕，重启启动控制通道闹钟

}

void handle_ftp(session_t* sess)
{
	ftp_reply(sess, FTP_GREET, "miniftpd 0.1");
	
	while (1)
	{
		memset(sess->cmdline, 0, sizeof(sess->cmdline));
		memset(sess->cmd, 0, sizeof(sess->cmd));
		memset(sess->args, 0, sizeof(sess->args));

		start_cmd_alarm(); //开启控制通道闹钟

		readline(sess->ctrl_fd, sess->cmdline, MAX_COMMAND_LINE);
		//去除\r\n
		str_trim_crlf(sess->cmdline);

		// 解析ftp命令和参数
		str_split(sess->cmdline, sess->cmd, sess->args, ' ');

		//将命令转换为大写
		str_upper(sess->cmd);

		//执行ftp命令
		const ftpcmd_t* ftp_ptr = ctrl_cmds;
		int found = 0;
		while (ftp_ptr->cmd != NULL)
		{
			if (strcmp(ftp_ptr->cmd, sess->cmd) == 0)
			{
				found = 1;

				if (ftp_ptr->cmd_handler != NULL)
					ftp_ptr->cmd_handler(sess);

				else
					ftp_reply(sess, FTP_COMMANDNOTIMPL, "Unimplement command.");

				break;
					
			}
	
			ftp_ptr++;
		}

		if (found == 1)
			continue;
		else
			ftp_reply(sess, FTP_BADCMD, "Unknown command.");
	}
}

void ftp_reply(session_t* sess, int status, const char* text)
{
	char buf[1024] = {0};
	sprintf(buf, "%d %s\r\n", status, text);
	writen(sess->ctrl_fd, buf, strlen(buf));
}

void ftp_lreply(session_t* sess, int status, const char* text)
{
	char buf[1024] = {0};
	sprintf(buf, "%d-%s\r\n", status, text);
	writen(sess->ctrl_fd, buf, strlen(buf));
}

int port_active(session_t* sess)
{
	if (sess->p_sock != NULL)
		return 1;

	return 0;
}

int pasv_active(session_t* sess)
{
	
	priv_sock_send_cmd(sess->ftp_fd, PRIV_SOCK_PASV_ACTIVE);
	int active = priv_sock_get_int(sess->ftp_fd);

	return active;
}

int get_port_fd(session_t* sess)
{
	priv_sock_send_cmd(sess->ftp_fd, PRIV_SOCK_GET_DATA_SOCK);
	unsigned short port = ntohs(sess->p_sock->sin_port);
	char *ip = inet_ntoa(sess->p_sock->sin_addr);
	priv_sock_send_int(sess->ftp_fd, (int)port);
	priv_sock_send_buf(sess->ftp_fd, ip, strlen(ip));
	
	char res = priv_sock_get_result(sess->ftp_fd);
	if (res == PRIV_SOCK_RESULT_BAD)
	{
		return 0;
	}
	else if (res == PRIV_SOCK_RESULT_OK)
	{
		sess->data_fd = priv_sock_recv_fd(sess->ftp_fd);
	}

	return 1;
}

int get_pasv_fd(session_t* sess)
{
	priv_sock_send_cmd(sess->ftp_fd, PRIV_SOCK_PASV_ACCEPT);
	char res = priv_sock_get_result(sess->ftp_fd);

	if (res == PRIV_SOCK_RESULT_BAD)
	{
		return 0;
	}
	else if (res == PRIV_SOCK_RESULT_OK)
	{
		sess->data_fd = priv_sock_recv_fd(sess->ftp_fd);
	}

	return 1;
}


int get_transfer_fd(session_t* sess)
{
	// 判断是否接收过PORT 或者PASV命令
	if (!port_active(sess) && !pasv_active(sess))
		return 0;

	// 如果是主动模式
	int ret = 1;
	if (port_active(sess))
	{
		if (pasv_active(sess))
		{
			fprintf(stderr, "both port and pasv are active.");
			exit(EXIT_FAILURE);
		}
		
		if (get_port_fd(sess) == 0)
			ret = 0;
	}
	
	// 如果是被动模式
	if (pasv_active(sess))
	{
		if (port_active(sess))
		{
			fprintf(stderr, "both port and pasv are active.");
			exit(EXIT_FAILURE);
		}

		if (get_pasv_fd(sess) == 0)
			ret = 0;
		
	}

	if (sess->p_sock)
	{
		free(sess->p_sock);
		sess->p_sock = NULL;
	}

	if (ret == 1)
	{
		start_data_alarm(); //数据通道创建完毕，开启数据通道闹钟
	}

	return ret;
}

static void do_user(session_t *sess)
{
	//USER simba
	struct passwd* pw = getpwnam(sess->args);
	if (pw == NULL)
	{
		ftp_reply(sess, FTP_LOGINERR, "Login incorrect.");
		return;
	}

	sess->uid = pw->pw_uid;
	ftp_reply(sess, FTP_GIVEPWORD, "Please specify passwd.");

}

static void do_pass(session_t *sess)
{
	// PASS 666666
	struct passwd* pw = getpwuid(sess->uid);
	if (pw == NULL)
	{
		ftp_reply(sess, FTP_LOGINERR, "Login incorrect.");
		return;
	}

	struct spwd* sp = getspnam(pw->pw_name);
	if (sp == NULL)
	{
		ftp_reply(sess, FTP_LOGINERR, "Login incorrect.");
		return;
	}

	//将客户端传递的用户密码加密后再比较
	char* encrypt = crypt(sess->args, sp->sp_pwdp);
	if (strcmp(encrypt, sp->sp_pwdp) != 0)
	{
		ftp_reply(sess, FTP_LOGINERR, "Login incorrect.");
		return;
	}

	signal(SIGURG, handle_sigurg);
	active_sigurg(sess->ctrl_fd);

	umask(tunable_local_umask);

	//将当前进程的有效用户更改为simba
	if (setegid(pw->pw_gid) < 0)
		ERR_EXIT("setegid");
	if (seteuid(pw->pw_uid) < 0)
		ERR_EXIT("seteuid");
	chdir(pw->pw_dir); //更改当前目录为用户家目录

	ftp_reply(sess, FTP_LOGINOK, "Login successful.");
	
}

static void do_cwd(session_t *sess)
{
	if (chdir(sess->args) < 0)
	{
		ftp_reply(sess, FTP_NOPERM, "Failed to change directory.");

		return;
	}

	ftp_reply(sess, FTP_CWDOK, "Directory successfully changed.");

}

static void do_cdup(session_t *sess)
{
	if (chdir("..") < 0)
	{
		ftp_reply(sess, FTP_NOPERM, "Failed to change directory.");

		return;
	}

	ftp_reply(sess, FTP_CWDOK, "Directory successfully changed.");
}

static void do_quit(session_t *sess)
{
	ftp_reply(sess, FTP_GOODBYE, "Goodbye.");
	exit(EXIT_SUCCESS);
}

static void do_port(session_t *sess)
{
	//  PORT 192,168,56,1,82,37
	unsigned int v[6];
	sscanf(sess->args, "%u,%u,%u,%u,%u,%u", &v[2],&v[3],&v[4],&v[5],&v[0],&v[1]);

	sess->p_sock = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
	memset(sess->p_sock, 0, sizeof(struct sockaddr_in));

	sess->p_sock->sin_family = AF_INET;
	unsigned char* p = (unsigned char*)&(sess->p_sock->sin_port);
	//网络字节序
	p[0] = v[0];
	p[1] = v[1];

	p = (unsigned char*)&(sess->p_sock->sin_addr);
	p[0] = v[2];
	p[1] = v[3];
	p[2] = v[4];
	p[3] = v[5];

	ftp_reply(sess, FTP_PORTOK, "PORT command successful. Consider using PASV.");

}

static void do_pasv(session_t *sess)
{

	// 响应 227 Entering Passive Mode (192,168,56,188,49,137).
	char ip[20] = {0};
	getlocalip(ip);

	priv_sock_send_cmd(sess->ftp_fd, PRIV_SOCK_PASV_LISTEN);
	unsigned short port = (unsigned short)priv_sock_get_int(sess->ftp_fd);


	unsigned int v[4];
	sscanf(ip, "%u.%u.%u.%u", &v[0], &v[1], &v[2], &v[3]);
	char buf[128] = {0};
	sprintf(buf, "Entering Passive Mode (%u,%u,%u,%u,%u,%u).", 
		v[0], v[1], v[2], v[3], port>>8, port&0xFF);

	ftp_reply(sess, FTP_PASVOK, buf);

}

static void do_type(session_t *sess)
{
	if (strcmp(sess->args, "A") == 0)
	{
		sess->is_ascii = 1;
		ftp_reply(sess, FTP_TYPEOK, "Switching to ASCII mode.");
	}

	else if (strcmp(sess->args, "I") == 0)
	{
		sess->is_ascii = 0;
		ftp_reply(sess, FTP_TYPEOK, "Switching to BINARY mode.");
	}

	else
	{
		ftp_reply(sess, FTP_BADCMD, "Unrecognised TYPE command.");
	}

}


static void do_stor(session_t *sess)
{
	upload_common(sess, 0);
}

static void do_appe(session_t *sess)
{
	upload_common(sess, 1);
}

static void do_list(session_t *sess)
{
	//创建数据连接
	if (get_transfer_fd(sess) == 0)
	{
		//425 响应
		ftp_reply(sess, FTP_BADSENDCONN, "Use PORT or PASV first.");
		return;
	}

	//150 响应
	ftp_reply(sess, FTP_DATACONN, "Here comes the directory listing.");

	//开始传输列表
	list_common(sess, 1);

	//传输结束，关闭数据连接
	close(sess->data_fd);
	sess->data_fd = -1;

	//226 应答
	ftp_reply(sess, FTP_TRANSFEROK, "Directory send OK.");
}

static void do_nlst(session_t *sess)
{
	//创建数据连接
	if (get_transfer_fd(sess) == 0)
	{
		//425 响应
		ftp_reply(sess, FTP_BADSENDCONN, "Use PORT or PASV first.");
		return;
	}

	//150 响应
	ftp_reply(sess, FTP_DATACONN, "Here comes the directory listing.");

	//开始传输列表
	list_common(sess, 0);

	//传输结束，关闭数据连接
	close(sess->data_fd);
	sess->data_fd = -1;

	//226 应答
	ftp_reply(sess, FTP_TRANSFEROK, "Directory send OK.");
}

static void do_rest(session_t *sess)
{
	sess->restart_pos = str_to_longlong(sess->args);
	char buf[1024] = {0};
	sprintf(buf, "Restart position accepted (%lld).", sess->restart_pos);
	
	ftp_reply(sess, FTP_RESTOK, buf);
}

static void do_abor(session_t *sess)
{
	ftp_reply(sess, FTP_ABOR_NOCONN, "No transfer to abor");
}

static void do_pwd(session_t *sess)
{
	char buf[1024] = {0};
	char home[1024] = {0};
	getcwd(home, sizeof(home));
	sprintf(buf, "\"%s\"", home);

	ftp_reply(sess, FTP_PWDOK, buf);
}

static void do_mkd(session_t *sess)
{
	if (mkdir(sess->args, 0777) < 0)
	{
		ftp_reply(sess, FTP_FILEFAIL, "Create directory operation fail.");
		return;
	}

	char text[4096] = {0};
	//绝对路径
	if (sess->args[0] == '/')
	{
		sprintf(text, "\"%s\" created", sess->args);
	}
	//相对路径
	else
	{
		char dir[4096] = {0};
		getcwd(dir, 4096);
		//判断是否以'/'结尾
		if (dir[strlen(dir)-1] == '/')
		{
			sprintf(text, "\"%s%s\" created", sess->args, dir);
		}
		else
			sprintf(text, "\"%s/%s\" created", sess->args, dir);
	}

	ftp_reply(sess, FTP_MKDIROK, text);

}

static void do_rmd(session_t *sess)
{
	if (rmdir(sess->args) < 0)
	{
		ftp_reply(sess, FTP_NOPERM, "Create directory operation fail.");
		return;
	}
	
	ftp_reply(sess, FTP_RMDIROK, "Create directory operation successful.");

}

static void do_dele(session_t *sess)
{
	if (unlink(sess->args) < 0)
	{
		ftp_reply(sess, FTP_NOPERM, "Delete operation fail.");
		return;
	}
	
	ftp_reply(sess, FTP_DELEOK, "Delete operation successful.");
}

static void do_rnfr(session_t *sess)
{
	sess->rnfr_name = (char*)malloc(strlen(sess->args)+1);
	memset(sess->rnfr_name, 0, strlen(sess->args)+1);
	strcpy(sess->rnfr_name, sess->args);
	
	ftp_reply(sess, FTP_RNFROK, "Ready for RNTO.");
}

static void do_rnto(session_t *sess)
{
	if (sess->rnfr_name == NULL)
	{
		ftp_reply(sess, FTP_NEEDRNFR, "RNFR require first.");
		return;
	}

	rename(sess->rnfr_name, sess->args);
	ftp_reply(sess, FTP_RENAMEOK, "Rename successful.");
	free(sess->rnfr_name);
	sess->rnfr_name = NULL; //置为NULL
}

static void do_site_chmod(session_t* sess, char* arg)
{
	if (strlen(arg) == 0)
	{
			ftp_reply(sess, FTP_BADCMD, "SITE CHMOD need 2 arguments.");
			return;
	}

	char perm[100] = {0};
	char file[100] = {0};
	str_split(arg, perm, file, ' ');
	if (strlen(file) == 0)
	{
		ftp_reply(sess, FTP_BADCMD, "SITE CHMOD need 2 arguments.");
		return;
	}
	
	unsigned int mode = str_octal_to_uint(perm);
	if (chmod(file, mode) < 0)
	{
		ftp_reply(sess, FTP_BADCMD, "SITE CHMOD Command fail.");
		return;
	}

	ftp_reply(sess, FTP_CHMODOK, "SITE CHMOD Command Ok.");

}

static void do_site_umask(session_t* sess, char* arg)
{
	if (strlen(arg) == 0)
	{
		char text[1024] = {0};
		sprintf(text, "Your current UMASK is 0%o", tunable_local_umask);
		ftp_reply(sess, FTP_UMASKOK, text);
		return;
	}

	unsigned int mode = str_octal_to_uint(arg);
	umask(mode);
	char text[1024] = {0};
	sprintf(text, "UMASK set to 0%o", mode);
	ftp_reply(sess, FTP_UMASKOK, text);

}

static void do_site(session_t *sess)
{
	char cmd[100] = {0};
	char arg[100] = {0};
	str_split(sess->args, cmd, arg, ' ');

	if (strcmp(cmd, "CHMOD") == 0)
	{
		 // SITE CHMOD <perm> <file>
		do_site_chmod(sess, arg);
	}

	else if (strcmp(cmd, "UMASK") == 0)
	{
		 // SITE UMASK [umask]
		do_site_umask(sess, arg);
	}

	else if (strcmp(cmd, "HELP") == 0)
	{
		 // SITE HELP
		ftp_reply(sess, FTP_SITEHELP, "CHMOD UMASK HELP");
	}

	else
		ftp_reply(sess, FTP_BADCMD, "Unknown SITE command.");

}

static void do_syst(session_t *sess)
{
	ftp_reply(sess, FTP_SYSTOK, "UNIX Type: L8");
}

static void do_feat(session_t *sess)
{
	ftp_lreply(sess, FTP_FEAT, "Features:");

	writen(sess->ctrl_fd, " EPRT\r\n", strlen(" EPRT\r\n"));
    writen(sess->ctrl_fd, " EPSV\r\n", strlen(" EPSV\r\n"));
    writen(sess->ctrl_fd, " MDTM\r\n", strlen(" MDTM\r\n"));
    writen(sess->ctrl_fd, " PASV\r\n", strlen(" PASV\r\n"));
	writen(sess->ctrl_fd, " REST STREAM\r\n", strlen(" REST STREAM\r\n"));
    writen(sess->ctrl_fd, " SIZE\r\n", strlen(" SIZE\r\n"));
    writen(sess->ctrl_fd, " TVFS\r\n", strlen(" TVFS\r\n"));
    writen(sess->ctrl_fd, " UTF8\r\n", strlen(" UTF8\r\n"));

   ftp_reply(sess, FTP_FEAT, "End");

}

static void do_size(session_t *sess)
{
	struct stat sbuf;
	if (stat(sess->args, &sbuf) < 0)
	{
		ftp_reply(sess, FTP_FILEFAIL, "SIZE operation fail.");
		return;
	}

	if (!S_ISREG(sbuf.st_mode))
	{
		ftp_reply(sess, FTP_FILEFAIL, "Could not get file size.");
		return;
	}
	
	char buf[20] = {0};
	sprintf(buf, "%lld", (long long)sbuf.st_size);
	ftp_reply(sess, FTP_SIZEOK, buf);
}

static void do_stat(session_t *sess)
{
	ftp_lreply(sess, FTP_STATOK, "FTP server status:");

	if (sess->upload_speed_max == 0)
	{
		writen(sess->ctrl_fd, "\t No session upload bandwidth limit\r\n",
		strlen("\t No session upload bandwidth limit\r\n"));
	}
	else if (sess->upload_speed_max > 0)
	{
		char text[1024] = {0};
		sprintf(text, "\t session upload bandwidth limit is %u b/s\r\n", sess->upload_speed_max);
		writen(sess->ctrl_fd, text, strlen(text));
	}

	if (sess->download_speed_max == 0)
	{
		writen(sess->ctrl_fd, "\t No session download bandwidth limit\r\n",
		strlen("\t No session download bandwidth limit\r\n"));
	}
	else if (sess->download_speed_max > 0)
	{
		char text[1024] = {0};
		sprintf(text, "\t session download bandwidth limit is %u b/s\r\n", sess->download_speed_max);
		writen(sess->ctrl_fd, text, strlen(text));
	}

	writen(sess->ctrl_fd, "\t Control connection is plain text\r\n",
		strlen("\t Control connection is plain text\r\n"));

	writen(sess->ctrl_fd, "\t Data connections will be plain text\r\n",
		strlen("\t Data connections will be plain text\r\n"));

	char text[1024] = {0};
		sprintf(text, "\t At session startup, client count was %u\r\n", sess->num_clients);
		writen(sess->ctrl_fd, text, strlen(text));

	writen(sess->ctrl_fd, "\t miniFTPd 0.1 - secure, fast, stable\r\n",
		strlen("\t miniFTPd 0.1 - secure, fast, stable\r\n"));

	ftp_reply(sess, FTP_STATOK, "End of status");
}

static void do_noop(session_t *sess)
{
	ftp_reply(sess, FTP_NOOPOK, "NOOP Ok.");
}

static void do_help(session_t *sess)
{
	ftp_lreply(sess, FTP_HELP, "The following commands are recognized.");
	writen(sess->ctrl_fd, " ABOR ACCT ALLO APPE CDUP CWD  DELE EPRT EPSV FEAT HELP LIST MDTM MKD\r\n",
		strlen(" ABOR ACCT ALLO APPE CDUP CWD  DELE EPRT EPSV FEAT HELP LIST MDTM MKD\r\n"));

	writen(sess->ctrl_fd, " MODE NLST NOOP OPTS PASS PASV PORT PWD  QUIT REIN REST RETR RMD  RNFR\r\n",
		strlen(" MODE NLST NOOP OPTS PASS PASV PORT PWD  QUIT REIN REST RETR RMD  RNFR\r\n"));

	writen(sess->ctrl_fd, " RNTO SITE SIZE SMNT STAT STOR STOU STRU SYST TYPE USER XCUP XCWD XMKD\r\n",
		strlen(" RNTO SITE SIZE SMNT STAT STOR STOU STRU SYST TYPE USER XCUP XCWD XMKD\r\n"));

	writen(sess->ctrl_fd, " XPWD XRMD\r\n",
		strlen(" XPWD XRMDr\n"));

	ftp_reply(sess, FTP_HELP, "Help OK.");
}

