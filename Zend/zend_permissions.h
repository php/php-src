#ifndef ZEND_PERMISSIONS_H
#define ZEND_PERMISSIONS_H

#include "zend.h"

BEGIN_EXTERN_C()

ZEND_API zend_result zend_validate_file_permission(
    zend_long permission,
    zend_long arg_num,
    const char *name
);

END_EXTERN_C()

#endif /* ZEND_PERMISSIONS_H */