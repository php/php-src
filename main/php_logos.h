#ifndef _PHP_LOGOS_H
#define _PHP_LOGOS_H

PHPAPI int php_register_info_logo(char *logo_string, char *mimetype, unsigned char *data, int size);
PHPAPI int php_unregister_info_logo(char *logo_string);
int php_init_info_logos(void);
int php_shutdown_info_logos(void);
int php_info_logos(const char *logo_string TSRMLS_DC);

#endif /* _PHP_LOGOS_H */
