/* This is a generated file, edit filter.stub.php instead.
 * Stub hash: f12b6f2b9560d0a9be085e020f603163cede9dd2 */

#ifndef ZEND_FILTER_DECL_f12b6f2b9560d0a9be085e020f603163cede9dd2_H
#define ZEND_FILTER_DECL_f12b6f2b9560d0a9be085e020f603163cede9dd2_H

typedef enum zend_enum_Filter_Filter {
	ZEND_ENUM_Filter_Filter_DEFAULT = 1,
	ZEND_ENUM_Filter_Filter_VALIDATE_BOOL = 2,
	ZEND_ENUM_Filter_Filter_VALIDATE_INT = 3,
	ZEND_ENUM_Filter_Filter_VALIDATE_FLOAT = 4,
	ZEND_ENUM_Filter_Filter_VALIDATE_REGEXP = 5,
	ZEND_ENUM_Filter_Filter_VALIDATE_DOMAIN = 6,
	ZEND_ENUM_Filter_Filter_VALIDATE_URL = 7,
	ZEND_ENUM_Filter_Filter_VALIDATE_EMAIL = 8,
	ZEND_ENUM_Filter_Filter_VALIDATE_IP = 9,
	ZEND_ENUM_Filter_Filter_VALIDATE_MAC = 10,
	ZEND_ENUM_Filter_Filter_SANITIZE_STRING = 11,
	ZEND_ENUM_Filter_Filter_SANITIZE_EMAIL = 12,
	ZEND_ENUM_Filter_Filter_SANITIZE_URL = 13,
	ZEND_ENUM_Filter_Filter_SANITIZE_NUMBER_INT = 14,
	ZEND_ENUM_Filter_Filter_SANITIZE_NUMBER_FLOAT = 15,
	ZEND_ENUM_Filter_Filter_SANITIZE_SPECIAL_CHARS = 16,
	ZEND_ENUM_Filter_Filter_SANITIZE_FULL_SPECIAL_CHARS = 17,
	ZEND_ENUM_Filter_Filter_SANITIZE_ADD_SLASHES = 18,
	ZEND_ENUM_Filter_Filter_CALLBACK = 19,
} zend_enum_Filter_Filter;

typedef enum zend_enum_Filter_Flag {
	ZEND_ENUM_Filter_Flag_NONE = 1,
	ZEND_ENUM_Filter_Flag_ALLOW_OCTAL = 2,
	ZEND_ENUM_Filter_Flag_ALLOW_HEX = 3,
	ZEND_ENUM_Filter_Flag_STRIP_LOW = 4,
	ZEND_ENUM_Filter_Flag_STRIP_HIGH = 5,
	ZEND_ENUM_Filter_Flag_STRIP_BACKTICK = 6,
	ZEND_ENUM_Filter_Flag_ENCODE_LOW = 7,
	ZEND_ENUM_Filter_Flag_ENCODE_HIGH = 8,
	ZEND_ENUM_Filter_Flag_ENCODE_AMP = 9,
	ZEND_ENUM_Filter_Flag_NO_ENCODE_QUOTES = 10,
	ZEND_ENUM_Filter_Flag_EMPTY_STRING_NULL = 11,
	ZEND_ENUM_Filter_Flag_ALLOW_FRACTION = 12,
	ZEND_ENUM_Filter_Flag_ALLOW_THOUSAND = 13,
	ZEND_ENUM_Filter_Flag_ALLOW_SCIENTIFIC = 14,
	ZEND_ENUM_Filter_Flag_PATH_REQUIRED = 15,
	ZEND_ENUM_Filter_Flag_QUERY_REQUIRED = 16,
	ZEND_ENUM_Filter_Flag_IPV4 = 17,
	ZEND_ENUM_Filter_Flag_IPV6 = 18,
	ZEND_ENUM_Filter_Flag_NO_RES_RANGE = 19,
	ZEND_ENUM_Filter_Flag_NO_PRIV_RANGE = 20,
	ZEND_ENUM_Filter_Flag_GLOBAL_RANGE = 21,
} zend_enum_Filter_Flag;

#endif /* ZEND_FILTER_DECL_f12b6f2b9560d0a9be085e020f603163cede9dd2_H */
