#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json.h"


int main(int argc, char **argv)
{
  struct json_object *new_obj;

  mc_set_debug(1);

  new_obj = json_tokener_parse("/* more difficult test case */ { \"glossary\": { \"title\": \"example glossary\", \"GlossDiv\": { \"title\": \"S\", \"GlossList\": [ { \"ID\": \"SGML\", \"SortAs\": \"SGML\", \"GlossTerm\": \"Standard Generalized Markup Language\", \"Acronym\": \"SGML\", \"Abbrev\": \"ISO 8879:1986\", \"GlossDef\": \"A meta-markup language, used to create markup languages such as DocBook.\", \"GlossSeeAlso\": [\"GML\", \"XML\", \"markup\"] } ] } } }");
  printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));
  json_object_put(new_obj);

  return 0;
}
