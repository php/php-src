#ifndef _MIME_H
#define _MIME_H

#include "php_globals.h"

void php3_mime_split(char *buf, int cnt, char *boundary, pval *http_post_vars PLS_DC);

#endif
