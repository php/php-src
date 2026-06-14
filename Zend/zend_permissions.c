#include "zend.h"
#include "zend_API.h"
#include "zend_exceptions.h"
#include "Zend/zend_permissions.h"

ZEND_API zend_result zend_validate_file_permission(
    zend_long permission,
    zend_long arg_num,
    const char *name
)
{
    if (permission < 0 || (permission & ~07777) != 0) {
        zend_argument_value_error(
            arg_num,
            "Invalid %s value (must be between 0 and 07777)",
            name
        );
        return FAILURE;
    }

    return SUCCESS;
}