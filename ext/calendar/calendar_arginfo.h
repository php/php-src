/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_cal_days_in_month, 0, 3, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, calendar, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, month, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, year, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_cal_from_jd, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, jd, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, calendar, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_cal_info, 0, 0, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, calendar, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_cal_to_jd, 0, 4, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, calendar, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, month, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, day, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, year, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_easter_date, 0, 0, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, year, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, method, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_easter_days arginfo_easter_date

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_frenchtojd, 0, 3, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, month, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, day, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, year, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_gregoriantojd arginfo_frenchtojd

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_jddayofweek, 0, 1, MAY_BE_LONG|MAY_BE_STRING)
	ZEND_ARG_TYPE_INFO(0, juliandaycount, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_jdmonthname, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, juliandaycount, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_jdtofrench, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, juliandaycount, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_jdtogregorian arginfo_jdtofrench

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_jdtojewish, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, juliandaycount, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, hebrew, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, fl, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_jdtojulian arginfo_jdtofrench

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_jdtounix, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, jday, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_jewishtojd arginfo_frenchtojd

#define arginfo_juliantojd arginfo_frenchtojd

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_unixtojd, 0, 0, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, timestamp, IS_LONG, 0)
ZEND_END_ARG_INFO()
