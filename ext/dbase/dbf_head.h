#include "php.h"

extern dbhead_t *get_dbf_head(int fd);
void free_dbf_head(dbhead_t *dbh);
extern int put_dbf_head(dbhead_t *dbh);
extern int get_dbf_field(dbhead_t *dbh, dbfield_t *dbf);
extern int put_dbf_field(dbhead_t *dbh, dbfield_t *dbf);
void put_dbf_info(dbhead_t *dbh);
extern char *get_dbf_f_fmt(dbfield_t *dbf);
extern dbhead_t *dbf_open(char *dp, int o_flags TSRMLS_DC);
void dbf_head_info(dbhead_t *dbh);
