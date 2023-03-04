/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 3660ad3239f93c84b6909c36ddfcc92dd0773c70 */

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

static void register_zlib_symbols(int module_number)
{
	REGISTER_LONG_CONSTANT("FORCE_GZIP", PHP_ZLIB_ENCODING_GZIP, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FORCE_DEFLATE", PHP_ZLIB_ENCODING_DEFLATE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_ENCODING_RAW", PHP_ZLIB_ENCODING_RAW, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_ENCODING_GZIP", PHP_ZLIB_ENCODING_GZIP, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_ENCODING_DEFLATE", PHP_ZLIB_ENCODING_DEFLATE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_NO_FLUSH", Z_NO_FLUSH, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_PARTIAL_FLUSH", Z_PARTIAL_FLUSH, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_SYNC_FLUSH", Z_SYNC_FLUSH, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_FULL_FLUSH", Z_FULL_FLUSH, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_BLOCK", Z_BLOCK, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_FINISH", Z_FINISH, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_FILTERED", Z_FILTERED, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_HUFFMAN_ONLY", Z_HUFFMAN_ONLY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_RLE", Z_RLE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_FIXED", Z_FIXED, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_DEFAULT_STRATEGY", Z_DEFAULT_STRATEGY, CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("ZLIB_VERSION", ZLIB_VERSION, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_VERNUM", ZLIB_VERNUM, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_OK", Z_OK, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_STREAM_END", Z_STREAM_END, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_NEED_DICT", Z_NEED_DICT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_ERRNO", Z_ERRNO, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_STREAM_ERROR", Z_STREAM_ERROR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_DATA_ERROR", Z_DATA_ERROR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_MEM_ERROR", Z_MEM_ERROR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_BUF_ERROR", Z_BUF_ERROR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_VERSION_ERROR", Z_VERSION_ERROR, CONST_PERSISTENT);
}

static zend_class_entry *register_class_InflateContext(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "InflateContext", class_InflateContext_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE;

	return class_entry;
}

static zend_class_entry *register_class_DeflateContext(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DeflateContext", class_DeflateContext_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE;

	return class_entry;
}
