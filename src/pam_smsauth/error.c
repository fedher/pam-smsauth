/*
 *  (C) 2006, J Federico Hernandez Scarso <fedehernandez@gmailcom>
 */
#include "error.h"
#include "pam_parse_args.h"
#include "sms.h"
#include "user.h"
#include "json_proto.h"

static const char *user_error_msg[] = {
	"User not specificed",		/* USER_NONE */
	"User info ok",			/* USER_INFO_OK */
	"User info Incomplete",		/* USER_INFO_ERR */
	"user_set_info(): null params", /* USER_EMPTYPARAMS_ERR */
	"Invalid gecos format",		/* USER_GECOSFORMAT_ERR */
	"User name not found", 		/* USER_NAME_ERR */
	"Area code not found",		/* USER_AREACODE_ERR */
	"Cel number not found",		/* USER_CELNUMBER_ERR */
	"Provider name not found"	/* USER_PROVIDER_ERR */	
};

static const char *parser_error_msg[] = {
	"Module without arguments",	/* PARSER_NULLARGS */
	"Arguments ok",			/* PARSER_OK */
	"Sock path error",		/* PARSER_SOCKPATH_ERR */
	"otp argument type error",	/* PARSER_OTPTYPE_ERR */
	"otp value type error",		/* PARSER_OTPTYPEVAL_ERR */
	"otp argument length error",	/* PARSER_OTPLEN_ERR */
	"otp value length error",	/* PARSER_OTPLENVAL_ERR */
	"except argument value error",	/* PARSER_EXCEPT_ERR */
	"time_expir argument error",	/* PARSER_TIMEEXPIR_ERR */
	"time_expir arguments value error",	/* PARSER_TIMEEXPIRVAL_ERR */
	"Argument not defined"		/* PARSER_ARGNOTDEFINED_ERR */
};

static const char *sms_error_msg[] = {
	"SMS init value",		/* SMS_NONE */
	"SMS sended ok",		/* SMS_OK */
	"Error on socket creation",	/* SMS_SOCKCREATE_ERR */
	"Json to string convertion error",	/* SMS_JSON2STR_ERR */
	"Couldn't write on socket",	/* SMS_WRITEONSOCK_ERR */
	"Couldn't read from socket",	/* SMS_READFROMSOCK_ERR */
	"snprintf error", 		/* SMS_SNPRINTF_ERR */
	"Couldn't connect to server",	/* SMS_CONN2SEVER_ERR */
	"The gateway couldn't send the sms or gateway is down" 
					/* SMS_GATEWAY_ERR */
};

static const char *json_proto_err_msg[] = {
	"JSON init value",			/* JSON_PACKET_NONE */
	"The gateway received the packet ok",	/* JSON_PACKET_RECV_OK */
	"The sms couldn't be sended by the gateway", /* JSON_PACKET_RECV_ERR */
	"JSON packet key couldn't be found", 	/* JSON_PACKET_KEY_NOTFOUND */
	"JSON packet field couldn't be found", /* JSON_PACKET_FIELD_NOTFOUND */
	"Parse error in JSON packet" 	/* JSON_PACKET_PARSE_ERR */
};

inline const char *
err2str(error_type_t type, int code)
{
	switch (type) {
	case USER:
		return user_error_msg[code];

	case PARSER:
		return parser_error_msg[code];
	case SMS:
		return sms_error_msg[code];
	case JSON_PROTO:
		return json_proto_err_msg[code];
	}
	return NULL;
}
