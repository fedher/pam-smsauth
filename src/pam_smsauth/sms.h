/*
 * (C) 2006, J. Federico Hernandez Scarso <fede.hernandez@gmail.com>
 */
#ifndef _SMS_H_
#define _SMS_H_

#include "json_proto.h"
#include "user.h"

#define SMS_MAXSIZE 24

typedef enum sms_result {
	SMS_NONE,
	SMS_OK,
	/* module internal failure */
	SMS_SOCKCREATE_ERR,
	SMS_JSON2STR_ERR,
	SMS_WRITEONSOCK_ERR,
	SMS_READFROMSOCK_ERR,
	SMS_SNPRINTF_ERR,
	/* server is down */
	SMS_CONN2SERVER_ERR,
	/* gateway couldn't send the sms */
	SMS_GATEWAY_ERR
} sms_result_t;

/**
 * sms_send()
 *
 * @sock_path
 * @cmd
 * @user
 * @return sms_result
 */
sms_result_t sms_send(const char *sock_path, json_cmd_t cmd, user_t *user);

/**
 * sms_on_err()
 *
 * @sock_path
 * @result
 * @user
 * @ctrl
 * @pam_ret
 * @return 0 on success, != 0 on failure
 */
int sms_on_err(const char *sock_path, sms_result_t result, user_t *user, int ctrl, 
		int *pam_ret);

#endif
