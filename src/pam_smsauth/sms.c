/*
 * (C) 2006, J. Federico Hernandez <fede.hernandez@gmail.com>
 */
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>	/* struct sockaddr_un */
#include <unistd.h>
#include <stdlib.h>

#include "sms.h"
#include "json_proto.h"
#include "pam_parse_args.h"

/**
 * wait4resp()
 *
 * @sockd
 *
 * @return 0 on success, < 0 on failure
 */
static sms_result_t
wait4resp(int sockd, unsigned int *id) {
	int nread = 0;
	char json_str[JSON_PACKET_MAXSIZE];
	json_packet_resp_t jsonp;

	memset(json_str, 0, JSON_PACKET_MAXSIZE);
	json_resp_init(&jsonp);

	if ((nread = read(sockd, json_str, sizeof json_str)) < 0)
		return SMS_READFROMSOCK_ERR;

	if (json_check_srv_resp(&jsonp, json_str, id) != JSON_PACKET_RECV_OK)
			return SMS_GATEWAY_ERR;

	return SMS_OK;
}

/**
 * socket_create()
 *
 * @srv_addr
 * @sock_path
 *
 * return socket descriptor or -1 on failure
 */
static int
socket_create(struct sockaddr_un *srv_addr, const char *sock_path) {
	int sockd;

	if ((sockd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
		return -1;
		
	bzero(srv_addr, sizeof (*srv_addr));
	
	srv_addr->sun_family = AF_LOCAL;
	strncpy(srv_addr->sun_path, sock_path, sizeof (srv_addr->sun_path)-1);

	return sockd;
}

#define RETURN_RET(val, sd) do { close(sd); return val; } while (0)

sms_result_t
sms_send(const char *sock_path, json_cmd_t cmd, user_t *user)
{
	struct sockaddr_un srv_addr;
	int sockd;
	int n = 0;
	json_packet_req_t jsonp;
	sms_result_t ret = SMS_NONE;
	char json_str[JSON_PACKET_MAXSIZE];
	char sms[SMS_MAXSIZE];
	static unsigned int id = 0;

	if ((sockd = socket_create(&srv_addr, sock_path)) < 0)
		RETURN_RET(SMS_SOCKCREATE_ERR, sockd);

	if (connect(sockd, (const struct sockaddr *)&srv_addr, 
				sizeof (srv_addr)) < 0)
		RETURN_RET(SMS_CONN2SERVER_ERR, sockd);

	memset(json_str, 0, sizeof json_str);
	memset(sms, 0, sizeof sms);
	if (snprintf(sms, sizeof sms, "** %s **", user->otp) < 0)
		RETURN_RET(SMS_SNPRINTF_ERR, sockd);

	json_req_init(&jsonp);
	json_req_set(&jsonp, JSON_PACKET_NAME, JSON_PACKET_VER, ++id, cmd, 
			user, sms);

	if ((n = json_req_2str(&jsonp, json_str)) < 0)
		RETURN_RET(SMS_JSON2STR_ERR, sockd);

	if (write(sockd, json_str, n) < 0)
		RETURN_RET(SMS_WRITEONSOCK_ERR, sockd);

	ret = wait4resp(sockd, &id);

	close(sockd);
	return ret;
}

int
sms_on_err(const char *sock_path, sms_result_t result, user_t *user, int ctrl, 
		int *pam_ret)
{
	/* pam_ret default value */
	*pam_ret = PAM_AUTH_ERR;

	switch (result) {
	/* Module internal failure */		
	case SMS_READFROMSOCK_ERR:

	case SMS_SOCKCREATE_ERR:

	case SMS_JSON2STR_ERR:

	case SMS_WRITEONSOCK_ERR:

	case SMS_SNPRINTF_ERR:
		*pam_ret = PAM_SYSTEM_ERR;
		return 1;

	/* The gateway couldn't send the otp. */		
	case SMS_GATEWAY_ERR:

	/* The server is down. */
	case SMS_CONN2SERVER_ERR:
		if (ctrl & PAM_SMSAUTH_RELAXED)
			*pam_ret = PAM_SUCCESS;
		return 2;

	/* The gateway sended the sms ok. */
	case SMS_OK:
		return 0;

	/* sms_result init value. */
	case SMS_NONE:
		return 3;
	}

	return 4;
}
