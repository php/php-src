#ifndef PHP_PDO_MYSQL_SQL_PARSER_H
#define PHP_PDO_MYSQL_SQL_PARSER_H
#include "php.h"
int pdo_parse_params(pdo_stmt_t *stmt, char *inquery, int inquery_len, char **outquery,
                        int *outquery_len);

#endif
