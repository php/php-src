/* JSON_parser.h */

#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include "php.h"
#include "ext/standard/php_smart_str.h"

#define JSON_PARSER_MAX_DEPTH 512

typedef struct JSON_parser_struct {
    int state;
    int depth;
    int top;
    int error;
    int* stack;
    zval *the_zstack[JSON_PARSER_MAX_DEPTH];

} * JSON_parser;

extern JSON_parser new_JSON_parser(int depth);
extern int parse_JSON(JSON_parser jp, zval *z, unsigned short utf16_json[], int length, int assoc TSRMLS_DC);
extern int free_JSON_parser(JSON_parser jp);
#endif
