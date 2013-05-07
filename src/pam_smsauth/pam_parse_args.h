/*
 * (C) 2006, J. Federico Hernandez Scarso <fede.hernandez@gmail.com>
 */
#ifndef _PAM_PARSE_ARGS_H_
#define _PAM_PARSE_ARGS_H_

#define PAM_SMSAUTH_DEBUG       0x01
#define PAM_SMSAUTH_RELAXED 	0x02
#define PAM_SMSAUTH_SOCKPATH	0X04
#define PAM_SMSAUTH_OTPLEN	0x08
#define PAM_SMSAUTH_OTPTYPE	0x10
#define PAM_SMSAUTH_EXCEPT	0x20
#define PAM_SMSAUTH_TIMEEXPIR 	0x40

#include <time.h>

#include "otp.h"

#define SOCK_PATH_MAXSIZE 	256
#define EXCEPT_USERNAME_MAXSIZE 24

typedef struct pam_smsauth_arg {
	int 		otp_len;
	otp_type_t	otp_type;
	char 		sock_path[SOCK_PATH_MAXSIZE];
	char		except_user[EXCEPT_USERNAME_MAXSIZE];
	time_t		time_expir;	
	int		flags;	/* debug, relaxed */ 
} pam_smsauth_arg_t;

typedef enum parser_result {
	PARSER_NULLARGS,
	PARSER_OK,
	PARSER_SOCKPATH_ERR,
	PARSER_OTPTYPE_ERR,
	PARSER_OTPTYPEVAL_ERR,
	PARSER_OTPLEN_ERR,
	PARSER_OTPLENVAL_ERR,
	PARSER_EXCEPT_ERR,
	PARSER_TIMEEXPIR_ERR,
	PARSER_TIMEEXPIRVAL_ERR,
	PARSER_ARGNOTDEFINED_ERR
} parser_result_t;

/**
 * pam_arg_init()
 *
 * @arg
 */
inline void pam_arg_init(pam_smsauth_arg_t *arg);

/**
 * pam_arg_parse()
 *
 * @arg
 * @argc
 * @argv
 * @flags
 *
 * @return PARSER_OK success, != PARSER_OK failure
 */
parser_result_t pam_arg_parse(pam_smsauth_arg_t *arg, int argc, 
		const char **argv, int *flags);

#endif
