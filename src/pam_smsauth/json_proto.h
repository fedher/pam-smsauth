/*
 * (C) 2006, J. Federico Hernandez Scarso <fede.hernandez@gmail.com>
 */
#ifndef _JSON_SMS_H_
#define _JSON_SMS_H_

#define JSON_PACKET_NAME "pam_smsauth"
#define JSON_PACKET_VER 1
#define JSON_PACKET_MAXSIZE 4096
#define JSON_NAME_MAXSIZE 12
#define JSON_CEL_MAXSIZE 28
#define JSON_SMS_MAXSIZE 100

typedef enum command {
	JSON_CMD_NONE, 
	JSON_CMD_REQ = 1, 
	JSON_CMD_RESP = 2
} json_cmd_t;

typedef enum json_resp {
	JSON_RESP_NONE,
	JSON_RESP_OK = 1,
	JSON_RESP_ERR = 2
} json_resp_t; 

typedef enum json_result {
	JSON_PACKET_NONE,
	JSON_PACKET_RECV_OK,
	JSON_PACKET_RECV_ERR,
	JSON_PACKET_KEY_NOTFOUND,
	JSON_PACKET_FIELD_NOTFOUND,
	JSON_PACKET_PARSE_ERR,
} json_result_t;

typedef struct json_packet_req {
	char		*name;
	int		ver;
	json_cmd_t 	cmd;
	unsigned int	id;
	char 		*provider;
	char		*cel_number;
	char		*sms;
} json_packet_req_t;

typedef struct json_packet_resp {
	char		*name;
	int		ver;
	json_cmd_t 	cmd;
	unsigned int	id;
	json_resp_t	result;
	char 		*msg;
} json_packet_resp_t;

/**
 * json_req_init()
 *
 * @req
 */
inline void json_req_init(json_packet_req_t *req);

/**
 * json_resp_init()
 *
 * @resp
 */
inline void json_resp_init(json_packet_resp_t *resp);

#include "user.h"

/**
 * json_req_set()
 *
 * @req
 * @name
 * @ver
 * @id
 * @cmd
 * @user
 * @sms
 */
inline void json_req_set(json_packet_req_t *req, char *name, int ver, 
		unsigned int id, json_cmd_t cmd, user_t *user, char *sms);

/**
 * json_resp_set()
 *
 * @resp - response json packet
 * @name - packet name
 * @ver	- version
 * @id - packet identification
 * @cmd	- command
 * @result - transaction result
 * @msg - transaction message
 */
inline void json_resp_set(json_packet_resp_t *resp, char *name, int ver, 
		unsigned int id, json_cmd_t cmd, json_resp_t result, char *msg);

/**
 *
 * json_req_2str(): Converts a json structure in a string.
 *
 * @r
 * @str
 * @return: json object length
 */
int json_req_2str(json_packet_req_t *r, char *str);


/**
 * json_check_srv_resp()
 *
 * @resp
 * @json_str
 * @id
 * @return JSON_PAKCET_RECV_OK on success
 */
json_result_t json_check_srv_resp(json_packet_resp_t *resp, char *json_str, 
		unsigned int *id);

#endif
