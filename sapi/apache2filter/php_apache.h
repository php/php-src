#ifndef PHP_APACHE_H
#define PHP_APACHE_H

typedef struct php_struct {
	int state;
	ap_bucket_brigade *bb;
	ap_filter_t *f;
} php_struct;

void *merge_php_config(apr_pool_t *p, void *base_conf, void *new_conf);
void *create_php_config(apr_pool_t *p, char *dummy);
void apply_config(void *);
extern const command_rec dir_cmds[];

#endif /* PHP_APACHE_H */
