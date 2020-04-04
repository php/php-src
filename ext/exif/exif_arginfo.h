/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_exif_tagname, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_exif_read_data, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_TYPE_INFO(0, sections_needed, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, sub_arrays, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, read_thumbnail, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_exif_thumbnail, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(1, width)
	ZEND_ARG_INFO(1, height)
	ZEND_ARG_INFO(1, imagetype)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_exif_imagetype, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()


ZEND_FUNCTION(exif_tagname);
ZEND_FUNCTION(exif_read_data);
ZEND_FUNCTION(exif_thumbnail);
ZEND_FUNCTION(exif_imagetype);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(exif_tagname, arginfo_exif_tagname)
	ZEND_FE(exif_read_data, arginfo_exif_read_data)
	ZEND_FE(exif_thumbnail, arginfo_exif_thumbnail)
	ZEND_FE(exif_imagetype, arginfo_exif_imagetype)
	ZEND_FE_END
};
