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
	ZEND_FE(ctype_alnum, arginfo_ctype_alnum)
	ZEND_FE(ctype_alpha, arginfo_ctype_alpha)
	ZEND_FE(ctype_cntrl, arginfo_ctype_cntrl)
	ZEND_FE(ctype_digit, arginfo_ctype_digit)
	ZEND_FE(ctype_lower, arginfo_ctype_lower)
	ZEND_FE(ctype_graph, arginfo_ctype_graph)
	ZEND_FE(ctype_print, arginfo_ctype_print)
	ZEND_FE(ctype_punct, arginfo_ctype_punct)
	ZEND_FE(ctype_space, arginfo_ctype_space)
	ZEND_FE(ctype_upper, arginfo_ctype_upper)
	ZEND_FE(ctype_xdigit, arginfo_ctype_xdigit)
	ZEND_FE_END
};
