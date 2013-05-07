/*
 * (C) 2006, J. Federico Hernandez Scarso <fede.hernandez@gmail.com>
 */
#ifndef _USER_H_
#define _USER_H_

#define USER_NAME_MAXSIZE 32
#define USER_PROVIDER_MAXSIZE 32
#define USER_AREACODE_MAXSIZE 10
#define USER_CELNUMBER_MAXSIZE 24

/**
 * User information.
 */
typedef struct user_info {
	char 	name[USER_NAME_MAXSIZE];
	char 	cel_number[USER_CELNUMBER_MAXSIZE];
	char	provider[USER_PROVIDER_MAXSIZE];
	char 	*otp;
} user_t;

typedef enum user_result {
	USER_NONE,
	USER_INFO_OK,
	USER_INFO_ERR,
	USER_EMPTYPARAMS_ERR,
	USER_GECOSFORMAT_ERR,
	USER_NAME_ERR,
	USER_AREACODE_ERR,
	USER_CELNUMBER_ERR,
	USER_PROVIDER_ERR
} user_result_t;

/**
 * user_init()
 *
 * @u
 */
inline void user_init(user_t *u);

/**
 * user_get_info(): retrieves the user data given the user name
 *
 * @user: user data
 * @user_name
 * @return: 0 success, 1 otherwise
 */
int user_get_info(user_t *user, const char * user_name);

#endif
