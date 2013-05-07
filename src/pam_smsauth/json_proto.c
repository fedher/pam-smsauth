/*
 *  (C) 2006, J. Federico Hernandez Scarso <fede.hernandez@gmail.com>
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "json_proto.h"
#include "json/json.h"
#include "json/json_object.h"
#include "json/json_object_private.h"

inline void 
json_req_init(json_packet_req_t *req)
{
	req->name = NULL;
	req->ver = 0;
	req->cmd = JSON_CMD_NONE;
	req->id = 0;
	req->provider = NULL;
	req->cel_number = NULL;
	req->sms = NULL;
}

inline void 
json_resp_init(json_packet_resp_t *resp)
{
	resp->name = NULL;
	resp->ver = 0;
	resp->cmd = JSON_CMD_NONE;
	resp->id = 0;
	resp->result = JSON_RESP_NONE;
	resp->msg = NULL;
}

inline void 
json_req_set(json_packet_req_t *req, char *name, int ver, unsigned int id,
		json_cmd_t cmd, user_t *user, char *sms)
{
	req->name = name;
	req->ver = ver;
	req->cmd = cmd;
	req->id = id;
	req->provider = user->provider;
	req->cel_number = user->cel_number;
	req->sms = sms;
}

inline void 
json_resp_set(json_packet_resp_t *resp, char *name, int ver, unsigned int id, 
		json_cmd_t cmd, json_resp_t result, char *msg)
{
	resp->name = name;
	resp->ver = ver;
	resp->cmd = cmd;
	resp->id = id;
	resp->result = result;
	resp->msg = msg;
}

int 
json_req_2str(json_packet_req_t *r, char *str)
{
	struct json_object *obj = NULL, *payload = NULL;
	int nprinted = 0;

	if ((obj = json_object_new_object()) == NULL)
		return -1;
	
	if ((payload = json_object_new_object()) == NULL)
		return -2;

	json_object_object_add(payload, "ver", json_object_new_int(r->ver));
	json_object_object_add(payload, "cmd", json_object_new_int(r->cmd));
	json_object_object_add(payload, "id", json_object_new_int(r->id));
	json_object_object_add(payload, "provider", 
			json_object_new_string(r->provider));
	json_object_object_add(payload, "cel_number", 
			json_object_new_string(r->cel_number));
	json_object_object_add(payload, "sms", json_object_new_string(r->sms));

	json_object_object_add(obj, r->name, payload);


	nprinted = snprintf(str, JSON_PACKET_MAXSIZE, "%s", 
			json_object_to_json_string(obj));

	/* Free mem. */
	json_object_put(payload);
	json_object_put(obj);

	return nprinted;
}

json_result_t
json_check_srv_resp(json_packet_resp_t *resp, char *json_str, unsigned int *id)
{
	struct json_object *obj = NULL, *payload = NULL;
	struct json_object_iter iter;
	int ret = -1;
	json_result_t result = JSON_PACKET_NONE;

	if ((obj = json_tokener_parse(json_str)) == NULL)
		return JSON_PACKET_PARSE_ERR;

	json_object_object_foreachC(obj, iter) {
		if (!strncmp(JSON_PACKET_NAME, iter.key, strlen(JSON_PACKET_NAME))) {
			payload = iter.val;
			break;
		}
		return JSON_PACKET_KEY_NOTFOUND;
	}

	json_object_object_foreachC(payload, iter) {
		if (!strncmp("result", iter.key, 6)) {
			ret = atoi(json_object_to_json_string(iter.val));
			result = (ret == 1) ? JSON_PACKET_RECV_OK : 
				JSON_PACKET_RECV_ERR;
		}
		if (!strncmp("id", iter.key, 2))
			*id = atoi(json_object_to_json_string(iter.val));
	}

	/* Free mem. */
	json_object_put(payload);
	json_object_put(obj);

	if (ret == -1)
		return JSON_PACKET_FIELD_NOTFOUND;

	return result;
}
