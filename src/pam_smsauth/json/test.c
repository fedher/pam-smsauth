#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json.h"


#define json_object_foreach(obj,key,val) \
 for(entry = json_object_get_object(obj)->head; (entry ? (key = (char*)entry->k, val = (struct json_object*)entry->v, entry) : 0); entry = entry->next)


int main(int argc, char **argv)
{
  struct json_object *obj, *array;
  char *key; struct json_object *val; struct lh_entry *entry;
 
  array = json_object_new_array();
  json_object_array_add(array, json_object_new_int(1));
  json_object_array_add(array, json_object_new_int(2));
  json_object_array_add(array, json_object_new_int(3));

  obj = json_object_new_object();
  json_object_object_add(obj, "abc", json_object_new_int(12));
  json_object_object_add(obj, "foo", json_object_new_string("bar"));
  json_object_object_add(obj, "bool0", json_object_new_boolean(0));
  json_object_object_add(obj, "bool1", json_object_new_boolean(1));
  json_object_object_add(obj, "baz", json_object_new_string("bang"));
  json_object_object_add(obj, "baz", json_object_new_string("fark"));
  json_object_object_add(obj, "arr", array);
//  json_object_object_del(obj, "baz");
 
  printf("obj=\n");
  json_object_foreach(obj, key, val) {
    printf("\t%s: %s\n", key, json_object_to_json_string(val));
  }
  
  printf("obj.to_string()=%s\n", json_object_to_json_string(obj));

  json_object_put(obj);
  json_object_put(array);

  return 0;
}
