#ifndef SAFE_MODE_H
#define SAFE_MODE_H

extern PHPAPI int php_checkuid(const char *filename, char *fopen_mode, int mode);
extern PHPAPI char *php_get_current_user(void);

#endif
