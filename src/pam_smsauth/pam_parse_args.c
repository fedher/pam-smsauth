/*
 * (C) 2006, J. Federico Hernandez Scarso <fede.hernandez@gmail.com>
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "pam_parse_args.h"
#include "otp.h"

#define BUF_MAXSIZE 32

/* Options */
#define DEBUG 		"debug"
#define OTPLEN 		"otplen"
#define OTPTYPE 	"otptype"
#define SOCKPATH 	"sockpath"
#define RELAXED 	"relaxed"
#define EXCEPT		"except"
#define TIME		"expir"

#define OTPTYPE_NUM	"num"
#define OTPTYPE_ALPHA	"alpha"
#define OTPTYPE_ALPHANUM "alphanum"

#define P_VAR "[^# =]"	/* variable name pattern */
#define P_EQU "[ =]"	/* pattern for the '=' sign and its surrounding spaces */
#define P_VAL "[^ \n]"  /* variable value pattern */

enum {
	OTPLEN_MIN=4,
	OTPLEN_MAX=24,
	OTPLEN_DEFAULT=5
};

enum {
	TIME_MIN=1,
	TIME_MAX=60,
	TIME_DEFAULT=5
};

inline void 
pam_arg_init(pam_smsauth_arg_t *pam_arg)
{
	pam_arg->otp_len 	= OTPLEN_DEFAULT;
	pam_arg->otp_type	= NUM;
	pam_arg->time_expir	= TIME_DEFAULT;
	pam_arg->flags 		= 0;

	strncpy(pam_arg->sock_path, "/var/run/smsauth/smsauth_sock", 
			SOCK_PATH_MAXSIZE);

	memset(pam_arg->except_user, 0, sizeof pam_arg->except_user);
}

/**
 * parse_args()
 *
 * @argv
 * @val: value
 * @return 0 success, != 0 failure
 */
static int
parse_args(const char *argv, char *val)
{
	int ret;
	char var[BUF_MAXSIZE];

	if (argv == NULL) return 1;

	ret=sscanf(argv, " %31" P_VAR "%*31" P_EQU "%31" P_VAL, var, val);

	if (ret == 2) return 0;

	return 2;
}

/**
 * set_otp_type()
 *
 * @arg
 * @val
 * @return 0 success, 1 failure
 */
static int 
set_otp_type(pam_smsauth_arg_t *pam_arg, char *val) 
{
	if (!strncmp(OTPTYPE_ALPHANUM, val, strlen(OTPTYPE_ALPHANUM)))
		pam_arg->otp_type = ALPHANUM;
	else if (!strncmp(OTPTYPE_ALPHA, val, strlen(OTPTYPE_ALPHA)))
		pam_arg->otp_type = ALPHA;
	else if (!strncmp(OTPTYPE_NUM, val, strlen(OTPTYPE_NUM)))
		pam_arg->otp_type = NUM;
	else 
		return 1;
	return 0;
}


parser_result_t
pam_arg_parse(pam_smsauth_arg_t *pam_arg, int argc, const char **argv, 
		int *flags)
{
	int max = 0;
	char val[BUF_MAXSIZE];

	for ( ; argc-- > 0; argv++) {
		memset(val, 0, sizeof val);

		if (!strncmp(DEBUG, *argv, strlen(DEBUG))) {
			*flags |= PAM_SMSAUTH_DEBUG;
			pam_arg->flags |= PAM_SMSAUTH_DEBUG;
			continue;
		} else if (!strncmp(SOCKPATH, *argv, strlen(SOCKPATH))) {
			if (parse_args(*argv, val) != 0)
				return PARSER_SOCKPATH_ERR;

			strncpy(pam_arg->sock_path, val, SOCK_PATH_MAXSIZE);
			*flags |= PAM_SMSAUTH_SOCKPATH;
			continue;
		} else if (!strncmp(RELAXED, *argv, strlen(RELAXED))) {
			pam_arg->flags |= PAM_SMSAUTH_RELAXED;
			*flags |= PAM_SMSAUTH_RELAXED;
			continue;
		} else if (!strncmp(OTPTYPE, *argv, strlen(OTPTYPE))) {
			if (parse_args(*argv, val) != 0)
				return PARSER_OTPTYPE_ERR;

			if (!isalpha((int)val[0]))
				return PARSER_OTPTYPEVAL_ERR;

			if (set_otp_type(pam_arg, val) != 0)
				return PARSER_OTPTYPEVAL_ERR;
	
			*flags |= PAM_SMSAUTH_OTPTYPE;	
			continue;
		} else if (!strncmp(OTPLEN, *argv, strlen(OTPLEN))) {
			if (parse_args(*argv, val) != 0)
				return PARSER_OTPLEN_ERR;

			if (!isdigit((int)val[0]))
				return PARSER_OTPLENVAL_ERR;

			max = atoi(val);		

			if (OTPLEN_MIN > max || max > OTPLEN_MAX)
				return PARSER_OTPLENVAL_ERR;

			pam_arg->otp_len = max;
			*flags |= PAM_SMSAUTH_OTPLEN;
			continue;
		} else if (!strncmp(EXCEPT, *argv, strlen(EXCEPT))) {
			if (parse_args(*argv, val) != 0)
				return PARSER_EXCEPT_ERR;

			strncpy(pam_arg->except_user, val, 
					EXCEPT_USERNAME_MAXSIZE);
			*flags |= PAM_SMSAUTH_EXCEPT;
			continue;
		} else if (!strncmp(TIME, *argv, strlen(TIME))) {
			if (parse_args(*argv, val) != 0)
				return PARSER_TIMEEXPIR_ERR;

			if (!isdigit((int)val[0]))
				return PARSER_TIMEEXPIRVAL_ERR;

			max = atoi(val);		

			if (TIME_MIN > max || max > TIME_MAX)
				return PARSER_TIMEEXPIRVAL_ERR;

			pam_arg->time_expir = max;
			*flags |= PAM_SMSAUTH_TIMEEXPIR;
			continue;
		} else {
			return PARSER_ARGNOTDEFINED_ERR;
		}
	}	
	return PARSER_OK;
}
