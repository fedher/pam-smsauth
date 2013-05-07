/*
 *  (C) 2006, J. Federico Hernandez Scarso <fede.hernandez@gmail.com>
 */
#ifndef _PAM_SMSAUTH_ERR_H
#define _PAM_SMSAUTH_ERR_H

typedef enum error_type {
	USER,
	PARSER,
	SMS,
	JSON_PROTO
} error_type_t;

/**
 * err2str()
 *
 * @error_type
 * @code
 * @return error code description
 */
inline const char *err2str(error_type_t type, int code);

#define GOTO_ERR(val, pamh, log_level, fmt...) do {	\
	ret=val; 					\
	pam_syslog(pamh, log_level, fmt);		\
	goto out; 					\
} while (0)

#define RETURN_RET(ret, pamh, fmt...) do { 	\
	pam_syslog(pamh, LOG_INFO, fmt);	\
	return ret; 				\
} while (0)

#define PAM_DEBUG(pamh, ctrl, fmt...) do {		\
	if (ctrl & PAM_SMSAUTH_DEBUG)			\
		pam_syslog(pamh, LOG_DEBUG, fmt);	\
} while (0)

#endif
