#include "session.h"
#include "nobody.h"
#include "ftpproto.h"
#include "privsock.h"
#include "sysutil.h"

void begin_session(session_t* sess)
{	
	active_oobinline(sess->ctrl_fd);

	priv_sock_init(sess);

	pid_t pid = fork();
	if (pid == -1)
		ERR_EXIT("fork");

	if (pid == 0)
	{
		// ftp协议解析进程
		priv_sock_set_child_context(sess);
		handle_ftp(sess);

	}
	else if (pid > 0)
	{
		// nobody进程是父进程
		priv_sock_set_parent_context(sess);
		handle_nobody(sess);

	}

}