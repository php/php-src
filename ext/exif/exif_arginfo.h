/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 633b2db018fa1453845a854a6361f11f107f4653 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_exif_tagname, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_exif_read_data, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, file)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, required_sections, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, as_arrays, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, read_thumbnail, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_exif_thumbnail, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, file)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, width, "null")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, height, "null")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, image_type, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_exif_imagetype, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_FUNCTION(exif_tagname);
ZEND_FUNCTION(exif_read_data);
ZEND_FUNCTION(exif_thumbnail);
ZEND_FUNCTION(exif_imagetype);

static const zend_function_entry ext_functions[] = {
	ZEND_RAW_FENTRY("exif_tagname", zif_exif_tagname, arginfo_exif_tagname, 0, NULL)
	ZEND_RAW_FENTRY("exif_read_data", zif_exif_read_data, arginfo_exif_read_data, 0, NULL)
	ZEND_RAW_FENTRY("exif_thumbnail", zif_exif_thumbnail, arginfo_exif_thumbnail, 0, NULL)
	ZEND_RAW_FENTRY("exif_imagetype", zif_exif_imagetype, arginfo_exif_imagetype, 0, NULL)
	ZEND_FE_END
};

static void register_exif_symbols(int module_number)
{
	REGISTER_LONG_CONSTANT("EXIF_USE_MBSTRING", USE_MBSTRING, CONST_PERSISTENT);
}
