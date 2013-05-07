/*
 * (C) 2006, J. Federico Hernandez Scarso <fede.hernandez@gmail.com>
 */
#ifndef _PAM_REGISTER_VAR_H_
#define _PAM_REGISTER_VAR_H_

#include <time.h>

/**
 * pam_register_init()
 *
 * @user
 * @timestamp
 * @return 0 on success, < 0 on error
 */
inline int pam_register_init(const char *user, time_t timestamp);

#include "sqlite_client.h"	/* data_type_t */

/**
 * pam_is_registered_var()
 *
 * @user
 * @var_name
 * @type
 * @return 1 if var_name exists, 0 otherwise
 */
inline int pam_is_registered_var(const char *user, const char *var_name, 
		data_type_t type);

/**
 * pam_register_otp()
 *
 * @user
 * @value
 * @return 1 on success, 0 otherwise
 */
inline int pam_register_otp(const char *user, const char *value);

/**
 * pam_unregister_expir_vars()
 *
 * @time
 * @return 0 on success, > 0 on error.
 */
inline int pam_unregister_expir_vars(time_t time);

/**
 * pam_get_str_var()
 *
 * @user
 * @var_name
 * @return pam on success, NULL on failure
 */
char *pam_get_str_var(const char *user, const char *var_name);

/**
 * pam_is_data_expir()
 *
 * @user
 * @timestamp
 * @return 1 if the data has expired, 0 if it hasn't expired, -1 on error
 */
inline int pam_is_data_expir(const char *user, time_t timestamp);

/**
 * pam_get_otp()
 *
 * @user
 * @return pam on success, NULL on failure
 */
inline char * pam_get_otp(const char *user);

#endif
