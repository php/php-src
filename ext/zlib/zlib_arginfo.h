/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 5862b97739c885589779f8ba3d13b4e390d72811 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ob_gzhandler, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_zlib_get_coding_type, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_gzfile, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, use_include_path, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gzopen, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, mode, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, use_include_path, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_readgzfile, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, use_include_path, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_zlib_encode, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, encoding, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, level, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_zlib_decode, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, max_length, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_gzdeflate, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, level, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_LONG, 0, "ZLIB_ENCODING_RAW")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_gzencode, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, level, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_LONG, 0, "ZLIB_ENCODING_GZIP")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_gzcompress, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, level, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_LONG, 0, "ZLIB_ENCODING_DEFLATE")
ZEND_END_ARG_INFO()

#define arginfo_gzinflate arginfo_zlib_decode

#define arginfo_gzdecode arginfo_zlib_decode

#define arginfo_gzuncompress arginfo_zlib_decode

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_gzwrite, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_gzputs arginfo_gzwrite

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gzrewind, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, stream)
ZEND_END_ARG_INFO()

#define arginfo_gzclose arginfo_gzrewind

#define arginfo_gzeof arginfo_gzrewind

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_gzgetc, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, stream)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gzpassthru, 0, 1, IS_LONG, 0)
	ZEND_ARG_INFO(0, stream)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gzseek, 0, 2, IS_LONG, 0)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, whence, IS_LONG, 0, "SEEK_SET")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_gztell, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, stream)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_gzread, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_gzgets, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_deflate_init, 0, 1, DeflateContext, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, encoding, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_deflate_add, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, context, DeflateContext, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flush_mode, IS_LONG, 0, "ZLIB_SYNC_FLUSH")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_inflate_init, 0, 1, InflateContext, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, encoding, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_inflate_add, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, context, InflateContext, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flush_mode, IS_LONG, 0, "ZLIB_SYNC_FLUSH")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_inflate_get_status, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, context, InflateContext, 0)
ZEND_END_ARG_INFO()

#define arginfo_inflate_get_read_len arginfo_inflate_get_status


ZEND_FUNCTION(ob_gzhandler);
ZEND_FUNCTION(zlib_get_coding_type);
ZEND_FUNCTION(gzfile);
ZEND_FUNCTION(gzopen);
ZEND_FUNCTION(readgzfile);
ZEND_FUNCTION(zlib_encode);
ZEND_FUNCTION(zlib_decode);
ZEND_FUNCTION(gzdeflate);
ZEND_FUNCTION(gzencode);
ZEND_FUNCTION(gzcompress);
ZEND_FUNCTION(gzinflate);
ZEND_FUNCTION(gzdecode);
ZEND_FUNCTION(gzuncompress);
ZEND_FUNCTION(fwrite);
ZEND_FUNCTION(rewind);
ZEND_FUNCTION(fclose);
ZEND_FUNCTION(feof);
ZEND_FUNCTION(fgetc);
ZEND_FUNCTION(fpassthru);
ZEND_FUNCTION(fseek);
ZEND_FUNCTION(ftell);
ZEND_FUNCTION(fread);
ZEND_FUNCTION(fgets);
ZEND_FUNCTION(deflate_init);
ZEND_FUNCTION(deflate_add);
ZEND_FUNCTION(inflate_init);
ZEND_FUNCTION(inflate_add);
ZEND_FUNCTION(inflate_get_status);
ZEND_FUNCTION(inflate_get_read_len);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(ob_gzhandler, arginfo_ob_gzhandler)
	ZEND_FE(zlib_get_coding_type, arginfo_zlib_get_coding_type)
	ZEND_FE(gzfile, arginfo_gzfile)
	ZEND_FE(gzopen, arginfo_gzopen)
	ZEND_FE(readgzfile, arginfo_readgzfile)
	ZEND_FE(zlib_encode, arginfo_zlib_encode)
	ZEND_FE(zlib_decode, arginfo_zlib_decode)
	ZEND_FE(gzdeflate, arginfo_gzdeflate)
	ZEND_FE(gzencode, arginfo_gzencode)
	ZEND_FE(gzcompress, arginfo_gzcompress)
	ZEND_FE(gzinflate, arginfo_gzinflate)
	ZEND_FE(gzdecode, arginfo_gzdecode)
	ZEND_FE(gzuncompress, arginfo_gzuncompress)
	ZEND_FALIAS(gzwrite, fwrite, arginfo_gzwrite)
	ZEND_FALIAS(gzputs, fwrite, arginfo_gzputs)
	ZEND_FALIAS(gzrewind, rewind, arginfo_gzrewind)
	ZEND_FALIAS(gzclose, fclose, arginfo_gzclose)
	ZEND_FALIAS(gzeof, feof, arginfo_gzeof)
	ZEND_FALIAS(gzgetc, fgetc, arginfo_gzgetc)
	ZEND_FALIAS(gzpassthru, fpassthru, arginfo_gzpassthru)
	ZEND_FALIAS(gzseek, fseek, arginfo_gzseek)
	ZEND_FALIAS(gztell, ftell, arginfo_gztell)
	ZEND_FALIAS(gzread, fread, arginfo_gzread)
	ZEND_FALIAS(gzgets, fgets, arginfo_gzgets)
	ZEND_FE(deflate_init, arginfo_deflate_init)
	ZEND_FE(deflate_add, arginfo_deflate_add)
	ZEND_FE(inflate_init, arginfo_inflate_init)
	ZEND_FE(inflate_add, arginfo_inflate_add)
	ZEND_FE(inflate_get_status, arginfo_inflate_get_status)
	ZEND_FE(inflate_get_read_len, arginfo_inflate_get_read_len)
	ZEND_FE_END
};


static const zend_function_entry class_InflateContext_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_DeflateContext_methods[] = {
	ZEND_FE_END
};
