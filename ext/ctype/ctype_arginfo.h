/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 155783e1858a7f24dbc1c3e810d5cffee5468bf7 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ctype_alnum, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, text, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_ctype_alpha arginfo_ctype_alnum

#define arginfo_ctype_cntrl arginfo_ctype_alnum

#define arginfo_ctype_digit arginfo_ctype_alnum

#define arginfo_ctype_lower arginfo_ctype_alnum

#define arginfo_ctype_graph arginfo_ctype_alnum

#define arginfo_ctype_print arginfo_ctype_alnum

#define arginfo_ctype_punct arginfo_ctype_alnum

#define arginfo_ctype_space arginfo_ctype_alnum

#define arginfo_ctype_upper arginfo_ctype_alnum

#define arginfo_ctype_xdigit arginfo_ctype_alnum

ZEND_FUNCTION(ctype_alnum);
ZEND_FUNCTION(ctype_alpha);
ZEND_FUNCTION(ctype_cntrl);
ZEND_FUNCTION(ctype_digit);
ZEND_FUNCTION(ctype_lower);
ZEND_FUNCTION(ctype_graph);
ZEND_FUNCTION(ctype_print);
ZEND_FUNCTION(ctype_punct);
ZEND_FUNCTION(ctype_space);
ZEND_FUNCTION(ctype_upper);
ZEND_FUNCTION(ctype_xdigit);

static const zend_function_entry ext_functions[] = {
	ZEND_RAW_FENTRY("ctype_alnum", zif_ctype_alnum, arginfo_ctype_alnum, 0, NULL)
	ZEND_RAW_FENTRY("ctype_alpha", zif_ctype_alpha, arginfo_ctype_alpha, 0, NULL)
	ZEND_RAW_FENTRY("ctype_cntrl", zif_ctype_cntrl, arginfo_ctype_cntrl, 0, NULL)
	ZEND_RAW_FENTRY("ctype_digit", zif_ctype_digit, arginfo_ctype_digit, 0, NULL)
	ZEND_RAW_FENTRY("ctype_lower", zif_ctype_lower, arginfo_ctype_lower, 0, NULL)
	ZEND_RAW_FENTRY("ctype_graph", zif_ctype_graph, arginfo_ctype_graph, 0, NULL)
	ZEND_RAW_FENTRY("ctype_print", zif_ctype_print, arginfo_ctype_print, 0, NULL)
	ZEND_RAW_FENTRY("ctype_punct", zif_ctype_punct, arginfo_ctype_punct, 0, NULL)
	ZEND_RAW_FENTRY("ctype_space", zif_ctype_space, arginfo_ctype_space, 0, NULL)
	ZEND_RAW_FENTRY("ctype_upper", zif_ctype_upper, arginfo_ctype_upper, 0, NULL)
	ZEND_RAW_FENTRY("ctype_xdigit", zif_ctype_xdigit, arginfo_ctype_xdigit, 0, NULL)
	ZEND_FE_END
};
