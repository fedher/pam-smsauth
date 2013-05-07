#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json.h"

#define json_object_foreach(obj,key,val) \
 for(entry = json_object_get_object(obj)->head; (entry ? (key = (char*)entry->k, val = (struct json_object*)entry->v, entry) : 0); entry = entry->next)

int main(int argc, char **argv)
{
  struct json_object *obj, *payload;
  char *key; struct json_object *val; struct lh_entry *entry;

  obj = json_tokener_parse("{ \"pam_smsauth\": { \"ver\": 1, \"cmd\": 1, \"provider\": \"movistar\", \"cel\": \"2615387138\", \"sms\": \"hula\" } }");

  json_object_foreach(obj, key, val) {
    printf("key: %s\nvalue: %s\n", 
		    key, 
		    json_object_to_json_string(val));
    payload = val;
  }

  json_object_foreach(payload, key, val) {
    printf("key: %s\nvalue: %s\n", 
		    key, 
		    json_object_to_json_string(val)); 
  }

  json_object_put(obj);
  json_object_put(payload);

  return 0;
}
