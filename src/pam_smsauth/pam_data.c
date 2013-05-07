/*
 * (C) 2006, J. Federico Hernandez Scarso <fede.hernandez@gmail.com>
 */
#include <string.h>
#include <security/pam_modules.h>

#include "pam_data.h"

const char *
pam_get_username(pam_handle_t *pamh)
{
	int ret = PAM_USER_UNKNOWN;
	const char *user_name = NULL;

	ret = pam_get_item(pamh, PAM_USER, (const void **)&user_name);
	if (ret != PAM_SUCCESS)
		ret = pam_get_item(pamh, PAM_RUSER, (const void **)&user_name);

	if (ret != PAM_SUCCESS)
		return NULL;

	return user_name; 
}

inline void
pam_data_init(pam_smsauth_data_t *data)
{
	data->pamh = NULL;
	data->user = NULL;
}

inline int
pam_data_set(pam_smsauth_data_t *data, pam_handle_t *pamh, char *err)
{
	data->pamh = pamh;
	if ((data->user = pam_get_username(pamh)) == NULL) {
		err = "E: user unknown.";
		return PAM_USER_UNKNOWN;
	}
	return PAM_SUCCESS;
}
