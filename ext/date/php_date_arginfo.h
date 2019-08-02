/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_strtotime, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, time, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, now, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_DateTimeInterface_format, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_DateTimeInterface_getTimezone, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_DateTimeInterface_getOffset arginfo_DateTimeInterface_getTimezone

#define arginfo_DateTimeInterface_getTimestamp arginfo_DateTimeInterface_getTimezone

ZEND_BEGIN_ARG_INFO_EX(arginfo_DateTimeInterface_diff, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, object, DateTimeInterface, 0)
	ZEND_ARG_TYPE_INFO(0, absolute, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_DateTimeInterface___wakeup arginfo_DateTimeInterface_getTimezone

ZEND_BEGIN_ARG_INFO_EX(arginfo_DateTime___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, time, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, timezone, DateTimeZone, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_DateTime___set_state, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_DateTime_createFromImmutable, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, object, DateTimeImmutable, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_DateTime_createFromFormat, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, time, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, timezone, DateTimeZone, 1)
ZEND_END_ARG_INFO()

#define arginfo_DateTime_getLastErrors arginfo_DateTimeInterface_getTimezone

ZEND_BEGIN_ARG_INFO_EX(arginfo_DateTime_modify, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, modify, IS_STRING, 0)
ZEND_END_ARG_INFO()
