/* JSON_parser.h */

#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include "php.h"
#include "ext/standard/php_smart_str.h"

#define JSON_PARSER_DEFAULT_DEPTH 512

typedef struct JSON_parser_struct {
    int state;
    int depth;
    int top;
	int error_code;
    int* stack;
    zval **the_zstack;
    zval *the_static_zstack[JSON_PARSER_DEFAULT_DEPTH];
} * JSON_parser;

enum error_codes {
	PHP_JSON_ERROR_NONE = 0,
    PHP_JSON_ERROR_DEPTH, 
    PHP_JSON_ERROR_STATE_MISMATCH,  
    PHP_JSON_ERROR_CTRL_CHAR,   
    PHP_JSON_ERROR_SYNTAX,
    PHP_JSON_ERROR_UTF8
};

extern JSON_parser new_JSON_parser(int depth);
extern int parse_JSON(JSON_parser jp, zval *z, unsigned short utf16_json[], int length, int assoc TSRMLS_DC);
extern int free_JSON_parser(JSON_parser jp);
#endif
