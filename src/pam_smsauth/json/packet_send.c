#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json.h"


#define json_object_foreach(obj,key,val) \
 for(entry = json_object_get_object(obj)->head; (entry ? (key = (char*)entry->k, val = (struct json_object*)entry->v, entry) : 0); entry = entry->next)


int main(int argc, char **argv)
{
	char *key; struct json_object *val; struct lh_entry *entry;
	struct json_object *obj = NULL, *payload = NULL;

	if ((obj = json_object_new_object()) == NULL)
		return -1;

	if ((payload = json_object_new_object()) == NULL)
		return -2;

	json_object_object_add(payload, "ver", json_object_new_int(1));
	json_object_object_add(payload, "cmd", json_object_new_int(1));
	json_object_object_add(payload, "provider", json_object_new_string("movistar"));
	json_object_object_add(payload, "cel", json_object_new_string("2615387138"));
	json_object_object_add(payload, "sms", json_object_new_string("hula"));

	json_object_object_add(obj, "pam_smsauth", payload);
/*
	printf("obj=\n");
	json_object_foreach(obj, key, val) {
		printf("\t%s: %s\n", key, json_object_to_json_string(val));
	}
*/
	printf("obj.to_string()=%s\n", json_object_to_json_string(obj));

	json_object_put(payload);
	json_object_put(obj);

	return 0;
}
