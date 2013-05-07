#ifndef _PAM_DATA_H_
#define _PAM_DATA_H_

#include <security/pam_modules.h>

/**
 * pam_get_username()
 *
 * @pamh
 * @return user_name or NULL
 */
const char * pam_get_username(pam_handle_t *pamh);

typedef struct pam_smsauth_data {
	pam_handle_t	*pamh;
	const char	*user;
} pam_smsauth_data_t;

/**
 * pam_data_init()
 *
 * @data
 */
inline void pam_data_init(pam_smsauth_data_t *data);

/**
 * pam_data_set()
 *
 * @data
 * @pamh
 * @err
 * @return PAM_SUCCESS on success, PAM_USER_UNKNOWN on error
 */
inline int pam_data_set(pam_smsauth_data_t *data, pam_handle_t *pamh, 
		char *err);

#endif
