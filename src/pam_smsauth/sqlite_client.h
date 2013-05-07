#ifndef _SQLITE_CLIENT_H_
#define _SQLITE_CLIENT_H_

#include <time.h>

typedef enum col_data_type { VARINT, VARCHAR } data_type_t;

/**
 * sqlite_insert_row()
 *
 * @user
 * @timestamp
 * @return 0 on success, < 0 on failure
 */
int sqlite_insert_row(const char *user, time_t timestamp);

/**
 * sqlite_is_field_empty()
 *
 * @user
 * @col_name
 * @type
 * @return 1 if var exists, 0 if it doesn't exist
 */
int sqlite_is_field_empty(const char *user, const char *col_name, 
		data_type_t type);

/**
 * sqlite_is_row_expir()
 *
 * @user
 * @timestamp
 * @return 1 if var has expired, 0 if it hasn't expired, -1 on error
 */
int sqlite_is_row_expir(const char *user, time_t timestamp);

/**
 * sqlite_set_col_str()
 *
 * @user
 * @col_name
 * @value
 * @return 0 on success, < 0 on failure
 */
int sqlite_set_col_str(const char *user, const char *col_name, 
		const char *value);

/**
 * sqlite_get_col_str(): the caller must exec free().
 *
 * @user
 * @col_name
 * @return value on success, NULL on failure
 */
char *sqlite_get_col_str(const char *user, const char *col_name);

/**
 * sqlite_set_col_int()
 *
 * @user
 * @col_name
 * @value
 * @return 0 on success, < 0 on failure
 */
int sqlite_set_col_int(const char *user, const char *col_name, long long value);

/**
 * sqlite_get_col_int()
 *
 * @user
 * @col_name
 * @return value on success, 0 on failure
 */
long long sqlite_get_col_int(const char *user, const char *col_name);

/**
 * sqlite_delete_row()
 *
 * @user
 * @return 0 on success, < 0  on failure
 */
int sqlite_delete_row(const char *user);

/**
 * pam_is_data_expir()
 *
 * @user
 * @timestamp
 * @return 1 if the data has expired, 0 if it hasn't expired
 */
int pam_is_data_expir(const char *user, time_t timestamp);

/**
 * sqlite_purge_table()
 *
 * @time
 * @return 0 on success, < 0 on failure
 */
int sqlite_purge_table(time_t time);

#endif
