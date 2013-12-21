#ifndef _FTPPROTO_H_
#define _FTPPROTO_H_
#include "session.h"

typedef void (*CMD_HANDLER)(session_t *sess);
typedef struct ftpcmd
{
	const char *cmd;
	CMD_HANDLER cmd_handler;
	
} ftpcmd_t;


void handle_ftp(session_t* sess);
void ftp_reply(session_t* sess, int status, const char* text);
void ftp_lreply(session_t* sess, int status, const char* text);


int get_transfer_fd(session_t*);
int get_port_fd(session_t* sess);
int get_pasv_fd(session_t* sess);

void list_common(session_t* sess, int detail);
void upload_common(session_t* sess, int is_append);


#endif