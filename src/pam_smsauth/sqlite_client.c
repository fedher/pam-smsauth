#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <ctype.h>

#include "sqlite_client.h"
#include "pam_syslog.h"
#include "pam_register_var.h"
#include "pam_data.h"

extern pam_smsauth_data_t pam_data;

#define VAR_MAXLEN 32

static int timeout_ms = 500;

#define DB_NAME "/var/lib/smsauth/pam_smsauth.db"
#define SQL_STATEMENT_MAXSIZE 256

#define ID_GOTO_ERR(val, pamh, fmt...)  do {	\
	pam_syslog(pamh, LOG_ERR, fmt); 	\
	id=val; 				\
	goto out; 				\
} while (0)

static int 
callback_rowid(void *id, int nfields, char **data_field, char **col_name)
{
	int i;
	for (i=0; i<nfields; i++) {
		if (data_field[i] == NULL) 
			return 1;
		*(int *)id = atoi(data_field[i]);
	}
	return 0;
}

#define SQL_SELECT_NEXT_ROWID "SELECT max(id) FROM pam_status;"

static int
sqlite_get_next_rowid(sqlite3 *db)
{
	int id = 0;
	int ret = SQLITE_OK;
	char *err_msg = NULL;
	
	/*
	 * The table id field can be empty, so SQLITE_EMPTY and SQLITE_ABORT 
	 * error codes must be ignored.
	 */
	ret = sqlite3_exec(db, SQL_SELECT_NEXT_ROWID, callback_rowid, &id, &err_msg);

	if (ret == SQLITE_BUSY)
		pam_syslog(pam_data.pamh, LOG_INFO, 
				"I: sqlite_get_next_rowid: busy.");

        if (ret != SQLITE_OK && ret != SQLITE_EMPTY && ret != SQLITE_ABORT)
		ID_GOTO_ERR(-1, pam_data.pamh, 
				"E: sqlite_get_next_rowid: error: %d.", ret);

out:
	if (err_msg != NULL)
		sqlite3_free(err_msg);	
	return id+1;
}

#define SQL_SELECT_ROWID "SELECT id FROM pam_status WHERE user='%s';"

static int
sqlite_get_rowid(sqlite3 *db, const char *user)
{
	int id = 0;
	int ret = SQLITE_OK;
	char *err_msg = NULL;
	char sql[SQL_STATEMENT_MAXSIZE];

	memset(sql, 0, sizeof sql);
	if (snprintf(sql, sizeof sql, SQL_SELECT_ROWID, user) < 0)
		ID_GOTO_ERR(-1, pam_data.pamh, "E: sqlite_get_rowid: snprintf");
	
	ret = sqlite3_exec(db, sql, callback_rowid, &id, &err_msg);

	if (ret == SQLITE_BUSY)
		pam_syslog(pam_data.pamh, LOG_INFO, 
				"I: sqlite_get_rowid: busy.");

        if (ret != SQLITE_OK)
		ID_GOTO_ERR(-2, pam_data.pamh, 
				"E: sqlite_get_rowid: error: %d.", ret);

out:
	if (err_msg != NULL)
		sqlite3_free(err_msg);	
	return id;
}

#define SQL_INSERT_DATA "INSERT INTO pam_status VALUES (%d,'%s',null,%ld);"

#define GOTO_ERR(val, pamh, fmt...) do { 	\
	ret=val; 				\
	pam_syslog(pamh, LOG_ERR, fmt);		\
	goto out;				\
} while (0)

int 
sqlite_insert_row(const char *user, time_t timestamp)
{
	sqlite3 *db = NULL;
	char *err_msg = 0;
	char sql[SQL_STATEMENT_MAXSIZE];
	int ret = 0;
	int result = SQLITE_ERROR;
	int rowid = 0;

	if (sqlite3_open(DB_NAME, &db))
		GOTO_ERR(-1, pam_data.pamh,
			"E: sqlite_insert_row: couldn't open database: %s",
			sqlite3_errmsg(db));

	sqlite3_busy_timeout(db, timeout_ms);
	
	if ((rowid = sqlite_get_next_rowid(db)) < 0)
		GOTO_ERR(-2, pam_data.pamh, 
				"E: sqlite_insert_row: couldn't get rowid.");

	memset(sql, 0, sizeof sql);
	if (snprintf(sql, sizeof sql, SQL_INSERT_DATA, rowid, user, 
				timestamp) < 0)
		GOTO_ERR(-3, pam_data.pamh, "E: sqlite_insert_row: snprintf.");

	result = sqlite3_exec(db, sql, NULL, NULL, &err_msg);
       
	if (result == SQLITE_BUSY)
		pam_syslog(pam_data.pamh, LOG_INFO, 
				"I: sqlite_insert_row: busy");

	if (result != SQLITE_OK)
		GOTO_ERR(-4, pam_data.pamh, "E: sqlite_insert_row: %s.", 
				err_msg);

out:
	if (err_msg != NULL)
		sqlite3_free(err_msg);
	sqlite3_close(db);
	return ret;
}

static int 
callback_data_str(void *result, int nfields, char **data_field, char **col_name)
{
	int i;
	int size = 0;

	for (i=0; i<nfields; i++) {
		if (data_field[i] == NULL) 
			return 1;
	
		size = strlen(data_field[i]);
		size = (size < VAR_MAXLEN) ? size : VAR_MAXLEN;

		memset(result, 0, size * sizeof (char));
		strncpy(result, data_field[i], size);
		
		*((char *)result + size) = '\0';
	}
	return 0;
}

#define DATA_GOTO_ERR(pamh, fmt...) do { 	\
	value=NULL; 				\
	pam_syslog(pamh, LOG_ERR, fmt);		\
	goto out; 				\
} while (0)

#define SQL_SELECT_DATA "SELECT %s FROM pam_status WHERE id=%d;"

char *
sqlite_get_col_str(const char *user, const char *col_name)
{
	sqlite3 *db = NULL;
	char sql[SQL_STATEMENT_MAXSIZE];
       	char *err_msg = NULL;
	char *value = NULL;
	int result = SQLITE_ERROR;
	int rowid = 0;

	if (sqlite3_open(DB_NAME, &db))
		DATA_GOTO_ERR(pam_data.pamh, 
				"E: sqlite_get_col_str: can't open db: %s.",
				sqlite3_errmsg(db));

	sqlite3_busy_timeout(db, timeout_ms);

	if ((value = malloc((VAR_MAXLEN + 1) * sizeof (char))) == NULL)
		DATA_GOTO_ERR(pam_data.pamh,
				"E: sqlite_get_col_str: malloc error.");

	if ((rowid = sqlite_get_rowid(db, user)) <= 0)
		DATA_GOTO_ERR(pam_data.pamh, 
				"N: sqlite_get_col_str: doesn't exist the rowid" 
				" for the user %s.", user);

	memset(sql, 0, sizeof sql);
	if (snprintf(sql, sizeof sql, SQL_SELECT_DATA, col_name, rowid) < 0)
		DATA_GOTO_ERR(pam_data.pamh, 
				"E: sqlite_get_col_str: snprintf error.");

	result = sqlite3_exec(db, sql, callback_data_str, value, &err_msg);

	switch (result) {
	case SQLITE_ABORT:
		if (value != NULL)
			free(value);
		value = NULL;
		break;

	case SQLITE_BUSY:
		pam_syslog(pam_data.pamh, LOG_INFO, 
				"I: sqlite_get_col_str: busy");
		break;

	case SQLITE_OK:
	case SQLITE_EMPTY:
		break;

	default:
		DATA_GOTO_ERR(pam_data.pamh,
				"E: sqlite_get_col_str: %d.", result);
	}

out:
	if (err_msg != NULL) 
		sqlite3_free(err_msg);
	sqlite3_close(db);
	return value;
}

static int 
callback_data_int(void *result, int nfields, char **data_field, char **col_name)
{
	int i;
	for (i=0; i<nfields; i++) {
		if (data_field[i] == NULL) 
			return 1;
		*(int *)result = atol(data_field[i]);
	}
	return 0;
}

long long
sqlite_get_col_int(const char *user, const char *col_name)
{
	sqlite3 *db = NULL;
	char sql[SQL_STATEMENT_MAXSIZE];
       	char *err_msg = NULL;
	long long ret = 0;
	int result = SQLITE_ERROR;
	int rowid = 0;

	if (sqlite3_open(DB_NAME, &db))
		GOTO_ERR(-1, pam_data.pamh, 
				"E: sqlite_get_col_int: can't open db: %s.",
				sqlite3_errmsg(db));

	sqlite3_busy_timeout(db, timeout_ms);

	if ((rowid = sqlite_get_rowid(db, user)) <= 0)
		GOTO_ERR(-2, pam_data.pamh, 
				"N: sqlite_get_col_int: doesn't exist one rowid" 
				" for the user %s.", user);

	memset(sql, 0, sizeof sql);
	if (snprintf(sql, sizeof sql, SQL_SELECT_DATA, col_name, rowid) < 0)
		GOTO_ERR(-3, pam_data.pamh, 
				"E: sqlite_get_col_int: snprintf error.");

	result = sqlite3_exec(db, sql, callback_data_int, &ret, &err_msg);

	if (result == SQLITE_BUSY)
		pam_syslog(pam_data.pamh, LOG_INFO, 
				"I: sqlite_get_col_int: busy");

        if ((result != SQLITE_OK) && (result != SQLITE_EMPTY))
		GOTO_ERR(-4, pam_data.pamh,
				"E: sqlite_get_col_int: %s.", err_msg);

out:
	if (err_msg != NULL) 
		sqlite3_free(err_msg);
	sqlite3_close(db);
	return ret;
}

#define SQL_UPDATE_STR_DATA "UPDATE pam_status SET %s='%s' WHERE id=%d;"

int 
sqlite_set_col_str(const char *user, const char *col_name, const char *value)
{
	sqlite3 *db = NULL;
	char *err_msg = 0;
	char sql[SQL_STATEMENT_MAXSIZE];
	int ret = 0;
	int result = SQLITE_ERROR;
	int rowid = 0;

	if (sqlite3_open(DB_NAME, &db))
		GOTO_ERR(-1, pam_data.pamh,
			"E: sqlite_set_col_str: couldn't open database: %s",
			sqlite3_errmsg(db));

	sqlite3_busy_timeout(db, timeout_ms);
	
	if ((rowid = sqlite_get_rowid(db, user)) <= 0)
		GOTO_ERR(-2, pam_data.pamh, 
				"E: sqlite_set_col_str: couldn't get rowid.");


	memset(sql, 0, sizeof sql);
	if (snprintf(sql, sizeof sql, SQL_UPDATE_STR_DATA, col_name, value, 
				rowid) < 0)
		GOTO_ERR(-3, pam_data.pamh, "E: sqlite_set_col_str: snprintf.");

	result = sqlite3_exec(db, sql, NULL, NULL, &err_msg);
       
	if (result == SQLITE_BUSY)
		pam_syslog(pam_data.pamh, LOG_INFO, 
				"I: sqlite_set_col_str: busy");

	if (result != SQLITE_OK)
		GOTO_ERR(-4, pam_data.pamh, 
				"E: sqlite_set_col_str: %s.", err_msg);

out:
	if (err_msg != NULL)
		sqlite3_free(err_msg);
	sqlite3_close(db);
	return ret;
}

#define SQL_UPDATE_INT_DATA "UPDATE pam_status SET %s=%lld WHERE id=%d;"

int 
sqlite_set_col_int(const char *user, const char *col_name, long long value)
{
	sqlite3 *db = NULL;
	char *err_msg = 0;
	char sql[SQL_STATEMENT_MAXSIZE];
	int ret = 0;
	int result = SQLITE_ERROR;
	int rowid = 0;

	if (sqlite3_open(DB_NAME, &db))
		GOTO_ERR(-1, pam_data.pamh,
			"E: sqlite_set_col_int: couldn't open database: %s",
			sqlite3_errmsg(db));

	sqlite3_busy_timeout(db, timeout_ms);
	
	if ((rowid = sqlite_get_rowid(db, user)) <= 0)
		GOTO_ERR(-2, pam_data.pamh, 
				"E: sqlite_set_col_int: couldn't get rowid.");

	memset(sql, 0, sizeof sql);
	if (snprintf(sql, sizeof sql, SQL_UPDATE_INT_DATA, col_name, value, 
				rowid) < 0)
		GOTO_ERR(-3, pam_data.pamh, "E: sqlite_set_col_int: snprintf.");

	result = sqlite3_exec(db, sql, NULL, NULL, &err_msg);
       
	if (result == SQLITE_BUSY)
		pam_syslog(pam_data.pamh, LOG_INFO, 
				"I: sqlite_set_col_int: busy");

	if (result != SQLITE_OK)
		GOTO_ERR(-4, pam_data.pamh, 
				"E: sqlite_set_col_int: %s.", err_msg);

out:
	if (err_msg != NULL)
		sqlite3_free(err_msg);
	sqlite3_close(db);
	return ret;
}

#define SQL_DELETE_ROW "DELETE FROM pam_status WHERE id=%d;"

int
sqlite_delete_row(const char *user)
{
	sqlite3 *db = NULL;
	char *err_msg = 0;
	char sql[SQL_STATEMENT_MAXSIZE];
	int ret = 0;
	int result = SQLITE_ERROR;
	int rowid = 0;

	if (sqlite3_open(DB_NAME, &db))
		GOTO_ERR(-1, pam_data.pamh,
			"E: sqlite_delete_row: couldn't open database: %s",
			sqlite3_errmsg(db));

	sqlite3_busy_timeout(db, timeout_ms);

	if ((rowid = sqlite_get_rowid(db, user)) < 0)
		GOTO_ERR(-2, pam_data.pamh, 
				"E: sqlite_delete_row: couldn't get rowid.");
	
	memset(sql, 0, sizeof sql);
	if (snprintf(sql, sizeof sql, SQL_DELETE_ROW, rowid) < 0)
		GOTO_ERR(-3, pam_data.pamh, "E: sqlite_delete_row: snprintf.");

	result = sqlite3_exec(db, sql, NULL, NULL, &err_msg);
       
	if (result == SQLITE_BUSY)
		pam_syslog(pam_data.pamh, LOG_INFO, 
				"I: sqlite_delete_row: busy");

	if (result != SQLITE_OK)
		GOTO_ERR(-4, pam_data.pamh, 
				"E: sqlite_delete_row: %s.", err_msg);

out:
	if (err_msg != NULL)
		sqlite3_free(err_msg);
	sqlite3_close(db);
	return ret;
}

int 
sqlite_is_field_empty(const char *user, const char *col_name, data_type_t type)
{
	switch (type) {
	case VARCHAR:
		if (sqlite_get_col_str(user, col_name) == NULL)
			return 1;
		break;
	case VARINT:
		if (sqlite_get_col_int(user, col_name) <= 0)
			return 1;
		break;
	}

	return 0;
}

int 
sqlite_is_row_expir(const char *user, time_t timestamp)
{
	long long tstamp = 0;

	if ((tstamp = sqlite_get_col_int(user, "timestamp")) <= 0)
		return -1;

	return (timestamp >= tstamp) ? 1 : 0;
}

#define SQL_PURGE_TABLE "DELETE FROM pam_status WHERE timestamp<%ld;"

int
sqlite_purge_table(time_t time)
{
	sqlite3 *db = NULL;
	char *err_msg = 0;
	char sql[SQL_STATEMENT_MAXSIZE];
	int ret = 0;
	int result = SQLITE_ERROR;

	if (sqlite3_open(DB_NAME, &db))
		GOTO_ERR(-1, pam_data.pamh,
			"E: sqlite_purge_table: couldn't open database: %s",
			sqlite3_errmsg(db));

	sqlite3_busy_timeout(db, timeout_ms);

	memset(sql, 0, sizeof sql);
	if (snprintf(sql, sizeof sql, SQL_PURGE_TABLE, time) < 0)
		GOTO_ERR(-2, pam_data.pamh, "E: sqlite_purge_table: snprintf.");

	result = sqlite3_exec(db, sql, NULL, NULL, &err_msg);
       
	if (result == SQLITE_BUSY)
		pam_syslog(pam_data.pamh, LOG_INFO, 
				"I: sqlite_purge_table: busy");

	if (result != SQLITE_OK)
		GOTO_ERR(-3, pam_data.pamh, 
				"E: sqlite_purge_table: %s.", err_msg);

out:
	if (err_msg != NULL)
		sqlite3_free(err_msg);
	sqlite3_close(db);
	return ret;
}
