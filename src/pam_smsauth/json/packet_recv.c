#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json.h"

#define json_object_foreach(obj,key,val) \
 for(entry = json_object_get_object(obj)->head; (entry ? (key = (char*)entry->k, val = (struct json_object*)entry->v, entry) : 0); entry = entry->next)

int main(int argc, char **argv)
{
	struct json_object *obj = NULL, *payload = NULL, *val = NULL;
	struct lh_entry *entry = NULL;
	char *key = NULL;
	int result = -1;

	//	obj = json_tokener_parse("{ \"pam_smsauth\": { \"ver\": 1, \"cmd\": 1, \"provider\": \"movistar\", \"cel\": \"2615387138\", \"sms\": \"hula\" } }");

	obj = json_tokener_parse("{ \"pam_smsauth\": { \"resu\": 1 } }");
	if (obj == NULL) {
		printf("parse error\n");
		return 1;
	}

	json_object_foreach(obj, key, val) {
		if (!strncmp("pam_smsauth", key, strlen("pam_smsauth"))) {
			payload = val;
			break;
		}
		printf("missed key\n");
		return 2;
	}

	json_object_foreach(payload, key, val) {
		if (!strncmp("result", key, 6))
			result = atoi(json_object_to_json_string(val));
	}

	printf("result: %d\n", result);

	if (result == -1)
		printf("missed field\n");

	json_object_put(payload);
	json_object_put(obj);

	return 0;
}
