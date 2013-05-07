/*
 * (C) 2006, J. Federico Hernandez Scarso <fede.hernandez@gmail.com>
 */
#include <stdio.h>

#include "pam_register_var.h"

inline int
pam_is_registered_var(const char *user, const char *var_name, data_type_t type)
{
	return !sqlite_is_field_empty(user, var_name, type);
}

inline int
pam_register_init(const char *user, time_t timestamp)
{
	return sqlite_insert_row(user, timestamp) ? 0 : 1;
}

inline int
pam_register_otp(const char *user, const char *value)
{
	return sqlite_set_col_str(user, "otp", value) ? 0 : 1;
}

inline int
pam_unregister_expir_vars(time_t time)
{
	return sqlite_purge_table(time) ? 0 : 1;
}

char * 
pam_get_str_var(const char *user, const char *var_name)
{
	return sqlite_get_col_str(user, var_name);
}

inline char * 
pam_get_otp(const char *user)
{
	return sqlite_get_col_str(user, "otp");
}

inline int
pam_is_data_expir(const char *user, time_t timestamp)
{
	return sqlite_is_row_expir(user, timestamp);
}
