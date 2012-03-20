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
#include "ossl.h"


/*
 * Classes
 */
VALUE cConfig;
VALUE eConfigError;

/*
 * Public
 */

/*
 * GetConfigPtr is a public C-level function for getting OpenSSL CONF struct
 * from an OpenSSL::Config(eConfig) instance.  We decided to implement
 * OpenSSL::Config in Ruby level but we need to pass native CONF struct for
 * some OpenSSL features such as X509V3_EXT_*.
 */
CONF *
GetConfigPtr(VALUE obj)
{
    CONF *conf;
    VALUE str;
    BIO *bio;
    long eline = -1;

    OSSL_Check_Kind(obj, cConfig);
    str = rb_funcall(obj, rb_intern("to_s"), 0);
    bio = ossl_obj2bio(str);
    conf = NCONF_new(NULL);
    if(!conf){
	BIO_free(bio);
	ossl_raise(eConfigError, NULL);
    }
    if(!NCONF_load_bio(conf, bio, &eline)){
	BIO_free(bio);
	NCONF_free(conf);
	if (eline <= 0) ossl_raise(eConfigError, "wrong config format");
	else ossl_raise(eConfigError, "error in line %d", eline);
	ossl_raise(eConfigError, NULL);
    }
    BIO_free(bio);

    return conf;
}


/*
 * INIT
 */
void
Init_ossl_config()
{
    char *default_config_file;
    eConfigError = rb_define_class_under(mOSSL, "ConfigError", eOSSLError);
    cConfig = rb_define_class_under(mOSSL, "Config", rb_cObject);

    default_config_file = CONF_get1_default_config_file();
    rb_define_const(cConfig, "DEFAULT_CONFIG_FILE",
		    rb_str_new2(default_config_file));
    OPENSSL_free(default_config_file);
    /* methods are defined by openssl/config.rb */
}
