/*
 * (C) 2006, J. Federico Hernandez Scarso <fede.hernandez@gmail.com>
 */
#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>
#include <shadow.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "pam_data.h"
#include "user.h"
#include "error.h"
#include "pam_syslog.h"

extern pam_smsauth_data_t pam_data;

static char *
pw_get_gecos4(const char *user_name) 
{
	struct passwd *user_data = NULL;

	if (user_name == NULL) 
		return NULL;

	if ((user_data = getpwnam(user_name)) == NULL)
		return NULL;

	return user_data->pw_gecos;
}

#define P_VAL "[^# ;]"	/* variable name pattern */
#define P_DELIM "[ ;]"	/* pattern for the ';' sign and its surrounding spaces */

static user_result_t
user_set_info(user_t *user, char *gecos)
{
	char user_name[USER_NAME_MAXSIZE]; 
	char area_code[USER_AREACODE_MAXSIZE];
	char cel_number[USER_CELNUMBER_MAXSIZE];
	char provider[USER_PROVIDER_MAXSIZE];
	int n = 0;

	memset(user_name, 0, sizeof user_name);
	memset(area_code, 0, sizeof area_code);
	memset(cel_number, 0, sizeof cel_number);
	memset(provider, 0, sizeof provider);

	if (user == NULL || gecos == NULL)
		return USER_EMPTYPARAMS_ERR;

	if ((gecos = strchr(gecos, '[')) == NULL)
		return USER_GECOSFORMAT_ERR;

	gecos+=1;

	n = sscanf(gecos, "%31" P_VAL "%*1" P_DELIM 
			"%9" P_VAL "%*1" P_DELIM 
			"%23" P_VAL "%*1" P_DELIM 
			"%31" P_VAL "%*1" P_DELIM, 
			user_name, area_code, cel_number, provider);

	if (n != 4)
		return USER_INFO_ERR;

	if (!isalpha((int)user_name[0]))
		return USER_NAME_ERR;
	
	strncpy(user->name, user_name, sizeof user->name);

	if (!isdigit((int)area_code[0]))
		return USER_AREACODE_ERR;
	
	if (!isdigit((int)cel_number[0]))
		return USER_CELNUMBER_ERR;
	
	if (snprintf(user->cel_number, USER_CELNUMBER_MAXSIZE, "%s%s", 
			area_code, cel_number) < 0)
		return USER_CELNUMBER_ERR;

	if (!isalpha((int)provider[0]))
		return USER_PROVIDER_ERR;

	if (strchr(provider, ']') != NULL)
		return USER_PROVIDER_ERR;

	strncpy(user->provider, provider, sizeof user->provider);

	return USER_INFO_OK;
}

inline void 
user_init(user_t *u) 
{
	memset(u->name, 0, USER_NAME_MAXSIZE);
	memset(u->cel_number, 0, USER_CELNUMBER_MAXSIZE);
	memset(u->provider, 0, USER_PROVIDER_MAXSIZE);
	u->otp = NULL;
}

int 
user_get_info(user_t *user, const char *user_name) 
{
	char *gecos = NULL;
	user_result_t val;
	int ret = 0;

	if ((gecos = pw_get_gecos4(user_name)) == NULL)
		GOTO_ERR(1, pam_data.pamh, LOG_ERR, "E: gecos null.");

	if ((val = user_set_info(user, gecos)) != USER_INFO_OK)
		GOTO_ERR(2, pam_data.pamh, LOG_ERR, err2str(USER, val));

out:	
	return ret;
}
