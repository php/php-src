/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: a5c534b7cd92619dfa0fdf29bd0a94fcda27f089 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_bzopen, 0, 0, 2)
	ZEND_ARG_INFO(0, file)
	ZEND_ARG_TYPE_INFO(0, mode, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_bzread, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, bz)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "1024")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_bzwrite, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, bz)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_bzflush, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, bz)
ZEND_END_ARG_INFO()

#define arginfo_bzclose arginfo_bzflush

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_bzerrno, 0, 1, IS_LONG, 0)
	ZEND_ARG_INFO(0, bz)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_bzerrstr, 0, 1, IS_STRING, 0)
	ZEND_ARG_INFO(0, bz)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_bzerror, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_INFO(0, bz)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_bzcompress, 0, 1, MAY_BE_STRING|MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, source, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, blocksize, IS_LONG, 0, "4")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, workfactor, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_bzdecompress, 0, 1, MAY_BE_STRING|MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, source, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, small, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()


ZEND_FUNCTION(bzopen);
ZEND_FUNCTION(bzread);
ZEND_FUNCTION(fwrite);
ZEND_FUNCTION(fflush);
ZEND_FUNCTION(fclose);
ZEND_FUNCTION(bzerrno);
ZEND_FUNCTION(bzerrstr);
ZEND_FUNCTION(bzerror);
ZEND_FUNCTION(bzcompress);
ZEND_FUNCTION(bzdecompress);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(bzopen, arginfo_bzopen)
	ZEND_FE(bzread, arginfo_bzread)
	ZEND_FALIAS(bzwrite, fwrite, arginfo_bzwrite)
	ZEND_FALIAS(bzflush, fflush, arginfo_bzflush)
	ZEND_FALIAS(bzclose, fclose, arginfo_bzclose)
	ZEND_FE(bzerrno, arginfo_bzerrno)
	ZEND_FE(bzerrstr, arginfo_bzerrstr)
	ZEND_FE(bzerror, arginfo_bzerror)
	ZEND_FE(bzcompress, arginfo_bzcompress)
	ZEND_FE(bzdecompress, arginfo_bzdecompress)
	ZEND_FE_END
};
