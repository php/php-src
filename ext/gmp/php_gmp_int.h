#ifndef incl_PHP_GMP_INT_H
#define incl_PHP_GMP_INT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"
#include <gmp.h>

#ifdef PHP_WIN32
# define PHP_GMP_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
# define PHP_GMP_API __attribute__ ((visibility("default")))
#else
# define PHP_GMP_API
#endif

typedef struct _gmp_object {
	mpz_t num;
	zend_object std;
} gmp_object;

static inline gmp_object *php_gmp_object_from_zend_object(zend_object *zobj) {
	return (gmp_object *)( ((char *)zobj) - XtOffsetOf(gmp_object, std) );
}

PHP_GMP_API zend_class_entry *php_gmp_class_entry(void);

/* GMP and MPIR use different datatypes on different platforms */
#ifdef _WIN64
typedef zend_long gmp_long;
typedef zend_ulong gmp_ulong;
#else
typedef long gmp_long;
typedef unsigned long gmp_ulong;
#endif

#endif
