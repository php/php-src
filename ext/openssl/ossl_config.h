/*
 * $Id$
 * 'OpenSSL for Ruby' project
 * Copyright (C) 2001-2002  Michal Rokos <m.rokos@sh.cvut.cz>
 * All rights reserved.
 */
/*
 * This program is licenced under the same licence as Ruby.
 * (See the file 'LICENCE'.)
 */
#if !defined(_OSSL_CONFIG_H_)
#define _OSSL_CONFIG_H_

extern VALUE cConfig;
extern VALUE eConfigError;

CONF* GetConfigPtr(VALUE obj);
CONF* DupConfigPtr(VALUE obj);
void Init_ossl_config(void);

#endif /* _OSSL_CONFIG_H_ */

