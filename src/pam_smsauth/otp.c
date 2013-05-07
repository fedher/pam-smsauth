/*
 * (C) 2006, J. Federico Hernandez Scarso <fede.hernandez@gmail.com>
 */
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <syslog.h>

#include <security/pam_modules.h>
#include <security/_pam_macros.h>

#include "pam_data.h"
#include "pam_prompt.h"
#include "pam_syslog.h"

#include "otp.h"

#define PROMPT "Token: "

extern pam_smsauth_data_t pam_data;

inline const char *
otp_get_from_user(pam_handle_t *pamh) 
{
	char *resp = NULL;
	pam_prompt(pamh, PAM_PROMPT_ECHO_ON, &resp, PROMPT);
	return resp;
}

inline int 
otp_cmp(const char *otp_input, const char *otp_gen, int len)
{
	return (strncmp(otp_gen, otp_input, len)) ? PAM_AUTH_ERR : PAM_SUCCESS;
}

#define GOTO_ERR(pamh, fmt...) do {	\
	pam_syslog(pamh, LOG_ERR, fmt); \
	otp = NULL; 			\
	goto err; 			\
} while (0)

static inline char *
select_otp_type(otp_type_t type) 
{
	char *alphanum = "0123456789abcdefghijklmnopqrstuvwxyz"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char *alpha = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char *num = "0123456789";

	switch (type) {
	case ALPHANUM:
		return alphanum;
	case ALPHA:
		return alpha;
	case NUM:
		return num;
	}
	return alphanum;
}

char *
otp_gen(int len, otp_type_t type) 
{
	long int number = 0; 
	int i, fd = 0;
	char *otp = NULL;
	char *otp_type = NULL;

	otp_type = select_otp_type(type);

	if ((otp = (char *)malloc((len+1) * sizeof (char))) == NULL)
		return NULL;

	if ((fd = open("/dev/urandom", O_RDONLY)) < 0)
		GOTO_ERR(pam_data.pamh, "E: can't open /dev/urandom.");
		
	for (i=0; i<len; i++) {
		if (read(fd, &number, sizeof number) < 0)
			GOTO_ERR(pam_data.pamh, "E: can't read /dev/urandom.");
		otp[i] = otp_type[number % strlen(otp_type)];
	}
	otp[len] = '\0';
	close(fd);
	return otp;

err:
	if (otp != NULL) free(otp);
	if (fd > 0) close(fd);
	return NULL;
}
