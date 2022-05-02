/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 5ebaf48b6736126648a981f5bbb25b0bf46dc22a */

ZEND_BEGIN_ARG_INFO_EX(arginfo_zip_open, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zip_close, 0, 1, IS_VOID, 0)
	ZEND_ARG_INFO(0, zip)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_zip_read, 0, 0, 1)
	ZEND_ARG_INFO(0, zip)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zip_entry_open, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, zip_dp)
	ZEND_ARG_INFO(0, zip_entry)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_STRING, 0, "\"rb\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zip_entry_close, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, zip_entry)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_zip_entry_read, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, zip_entry)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, len, IS_LONG, 0, "1024")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_zip_entry_name, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, zip_entry)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_zip_entry_compressedsize, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, zip_entry)
ZEND_END_ARG_INFO()

#define arginfo_zip_entry_filesize arginfo_zip_entry_compressedsize

#define arginfo_zip_entry_compressionmethod arginfo_zip_entry_name

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_open, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_setPassword, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_close, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ZipArchive_count arginfo_class_ZipArchive_close

#define arginfo_class_ZipArchive_getStatusString arginfo_class_ZipArchive_close

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_addEmptyDir, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, dirname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_addFromString, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, content, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "ZipArchive::FL_OVERWRITE")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_addFile, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filepath, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, entryname, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, start, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "ZipArchive::FL_OVERWRITE")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_replaceFile, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, filepath, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, start, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_addGlob, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_addPattern, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, path, IS_STRING, 0, "\".\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_renameIndex, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, new_name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_renameName, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, new_name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_setArchiveComment, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, comment, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_getArchiveComment, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_setCommentIndex, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, comment, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_setCommentName, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, comment, IS_STRING, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_SET_MTIME)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_setMtimeIndex, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, timestamp, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_SET_MTIME)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_setMtimeName, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, timestamp, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_getCommentIndex, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_getCommentName, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_deleteIndex, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_deleteName, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ZipArchive_statName arginfo_class_ZipArchive_getCommentName

#define arginfo_class_ZipArchive_statIndex arginfo_class_ZipArchive_getCommentIndex

#define arginfo_class_ZipArchive_locateName arginfo_class_ZipArchive_getCommentName

#define arginfo_class_ZipArchive_getNameIndex arginfo_class_ZipArchive_getCommentIndex

#define arginfo_class_ZipArchive_unchangeArchive arginfo_class_ZipArchive_close

#define arginfo_class_ZipArchive_unchangeAll arginfo_class_ZipArchive_close

#define arginfo_class_ZipArchive_unchangeIndex arginfo_class_ZipArchive_deleteIndex

#define arginfo_class_ZipArchive_unchangeName arginfo_class_ZipArchive_deleteName

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_extractTo, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, pathto, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, files, MAY_BE_ARRAY|MAY_BE_STRING|MAY_BE_NULL, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_getFromName, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, len, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_getFromIndex, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, len, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_ZipArchive_getStream arginfo_class_ZipArchive_deleteName

#if defined(ZIP_OPSYS_DEFAULT)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_setExternalAttributesName, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, opsys, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, attr, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

#if defined(ZIP_OPSYS_DEFAULT)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_setExternalAttributesIndex, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, opsys, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, attr, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

#if defined(ZIP_OPSYS_DEFAULT)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_getExternalAttributesName, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_INFO(1, opsys)
	ZEND_ARG_INFO(1, attr)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

#if defined(ZIP_OPSYS_DEFAULT)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_getExternalAttributesIndex, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_INFO(1, opsys)
	ZEND_ARG_INFO(1, attr)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_setCompressionName, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, method, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, compflags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_setCompressionIndex, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, method, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, compflags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#if defined(HAVE_ENCRYPTION)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_setEncryptionName, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, method, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, password, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_ENCRYPTION)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_setEncryptionIndex, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, method, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, password, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_PROGRESS_CALLBACK)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_registerProgressCallback, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, rate, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_CANCEL_CALLBACK)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_registerCancelCallback, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_METHOD_SUPPORTED)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ZipArchive_isCompressionMethodSupported, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, method, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, enc, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_METHOD_SUPPORTED)
#define arginfo_class_ZipArchive_isEncryptionMethodSupported arginfo_class_ZipArchive_isCompressionMethodSupported
#endif


ZEND_FUNCTION(zip_open);
ZEND_FUNCTION(zip_close);
ZEND_FUNCTION(zip_read);
ZEND_FUNCTION(zip_entry_open);
ZEND_FUNCTION(zip_entry_close);
ZEND_FUNCTION(zip_entry_read);
ZEND_FUNCTION(zip_entry_name);
ZEND_FUNCTION(zip_entry_compressedsize);
ZEND_FUNCTION(zip_entry_filesize);
ZEND_FUNCTION(zip_entry_compressionmethod);
ZEND_METHOD(ZipArchive, open);
ZEND_METHOD(ZipArchive, setPassword);
ZEND_METHOD(ZipArchive, close);
ZEND_METHOD(ZipArchive, count);
ZEND_METHOD(ZipArchive, getStatusString);
ZEND_METHOD(ZipArchive, addEmptyDir);
ZEND_METHOD(ZipArchive, addFromString);
ZEND_METHOD(ZipArchive, addFile);
ZEND_METHOD(ZipArchive, replaceFile);
ZEND_METHOD(ZipArchive, addGlob);
ZEND_METHOD(ZipArchive, addPattern);
ZEND_METHOD(ZipArchive, renameIndex);
ZEND_METHOD(ZipArchive, renameName);
ZEND_METHOD(ZipArchive, setArchiveComment);
ZEND_METHOD(ZipArchive, getArchiveComment);
ZEND_METHOD(ZipArchive, setCommentIndex);
ZEND_METHOD(ZipArchive, setCommentName);
#if defined(HAVE_SET_MTIME)
ZEND_METHOD(ZipArchive, setMtimeIndex);
#endif
#if defined(HAVE_SET_MTIME)
ZEND_METHOD(ZipArchive, setMtimeName);
#endif
ZEND_METHOD(ZipArchive, getCommentIndex);
ZEND_METHOD(ZipArchive, getCommentName);
ZEND_METHOD(ZipArchive, deleteIndex);
ZEND_METHOD(ZipArchive, deleteName);
ZEND_METHOD(ZipArchive, statName);
ZEND_METHOD(ZipArchive, statIndex);
ZEND_METHOD(ZipArchive, locateName);
ZEND_METHOD(ZipArchive, getNameIndex);
ZEND_METHOD(ZipArchive, unchangeArchive);
ZEND_METHOD(ZipArchive, unchangeAll);
ZEND_METHOD(ZipArchive, unchangeIndex);
ZEND_METHOD(ZipArchive, unchangeName);
ZEND_METHOD(ZipArchive, extractTo);
ZEND_METHOD(ZipArchive, getFromName);
ZEND_METHOD(ZipArchive, getFromIndex);
ZEND_METHOD(ZipArchive, getStream);
#if defined(ZIP_OPSYS_DEFAULT)
ZEND_METHOD(ZipArchive, setExternalAttributesName);
#endif
#if defined(ZIP_OPSYS_DEFAULT)
ZEND_METHOD(ZipArchive, setExternalAttributesIndex);
#endif
#if defined(ZIP_OPSYS_DEFAULT)
ZEND_METHOD(ZipArchive, getExternalAttributesName);
#endif
#if defined(ZIP_OPSYS_DEFAULT)
ZEND_METHOD(ZipArchive, getExternalAttributesIndex);
#endif
ZEND_METHOD(ZipArchive, setCompressionName);
ZEND_METHOD(ZipArchive, setCompressionIndex);
#if defined(HAVE_ENCRYPTION)
ZEND_METHOD(ZipArchive, setEncryptionName);
#endif
#if defined(HAVE_ENCRYPTION)
ZEND_METHOD(ZipArchive, setEncryptionIndex);
#endif
#if defined(HAVE_PROGRESS_CALLBACK)
ZEND_METHOD(ZipArchive, registerProgressCallback);
#endif
#if defined(HAVE_CANCEL_CALLBACK)
ZEND_METHOD(ZipArchive, registerCancelCallback);
#endif
#if defined(HAVE_METHOD_SUPPORTED)
ZEND_METHOD(ZipArchive, isCompressionMethodSupported);
#endif
#if defined(HAVE_METHOD_SUPPORTED)
ZEND_METHOD(ZipArchive, isEncryptionMethodSupported);
#endif


static const zend_function_entry ext_functions[] = {
	ZEND_DEP_FE(zip_open, arginfo_zip_open)
	ZEND_DEP_FE(zip_close, arginfo_zip_close)
	ZEND_DEP_FE(zip_read, arginfo_zip_read)
	ZEND_DEP_FE(zip_entry_open, arginfo_zip_entry_open)
	ZEND_DEP_FE(zip_entry_close, arginfo_zip_entry_close)
	ZEND_DEP_FE(zip_entry_read, arginfo_zip_entry_read)
	ZEND_DEP_FE(zip_entry_name, arginfo_zip_entry_name)
	ZEND_DEP_FE(zip_entry_compressedsize, arginfo_zip_entry_compressedsize)
	ZEND_DEP_FE(zip_entry_filesize, arginfo_zip_entry_filesize)
	ZEND_DEP_FE(zip_entry_compressionmethod, arginfo_zip_entry_compressionmethod)
	ZEND_FE_END
};


static const zend_function_entry class_ZipArchive_methods[] = {
	ZEND_ME(ZipArchive, open, arginfo_class_ZipArchive_open, ZEND_ACC_PUBLIC)
	ZEND_ME(ZipArchive, setPassword, arginfo_class_ZipArchive_setPassword, ZEND_ACC_PUBLIC)
	ZEND_ME(ZipArchive, close, arginfo_class_ZipArchive_close, ZEND_ACC_PUBLIC)
	ZEND_ME(ZipArchive, count, arginfo_class_ZipArchive_count, ZEND_ACC_PUBLIC)
	ZEND_ME(ZipArchive, getStatusString, arginfo_class_ZipArchive_getStatusString, ZEND_ACC_PUBLIC)
	ZEND_ME(ZipArchive, addEmptyDir, arginfo_class_ZipArchive_addEmptyDir, ZEND_ACC_PUBLIC)
	ZEND_ME(ZipArchive, addFromString, arginfo_class_ZipArchive_addFromString, ZEND_ACC_PUBLIC)
	ZEND_ME(ZipArchive, addFile, arginfo_class_ZipArchive_addFile, ZEND_ACC_PUBLIC)
	ZEND_ME(ZipArchive, replaceFile, arginfo_class_ZipArchive_replaceFile, ZEND_ACC_PUBLIC)
	ZEND_ME(ZipArchive, addGlob, arginfo_class_ZipArchive_addGlob, ZEND_ACC_PUBLIC)
	ZEND_ME(ZipArchive, addPattern, arginfo_class_ZipArchive_addPattern, ZEND_ACC_PUBLIC)
	ZEND_ME(ZipArchive, renameIndex, arginfo_class_ZipArchive_renameIndex, ZEND_ACC_PUBLIC)
	ZEND_ME(ZipArchive, renameName, arginfo_class_ZipArchive_renameName, ZEND_ACC_PUBLIC)
	ZEND_ME(ZipArchive, setArchiveComment, arginfo_class_ZipArchive_setArchiveComment, ZEND_ACC_PUBLIC)
	ZEND_ME(ZipArchive, getArchiveComment, arginfo_class_ZipArchive_getArchiveComment, ZEND_ACC_PUBLIC)
	ZEND_ME(ZipArchive, setCommentIndex, arginfo_class_ZipArchive_setCommentIndex, ZEND_ACC_PUBLIC)
	ZEND_ME(ZipArchive, setCommentName, arginfo_class_ZipArchive_setCommentName, ZEND_ACC_PUBLIC)
#if defined(HAVE_SET_MTIME)
	ZEND_ME(ZipArchive, setMtimeIndex, arginfo_class_ZipArchive_setMtimeIndex, ZEND_ACC_PUBLIC)
#endif
#if defined(HAVE_SET_MTIME)
	ZEND_ME(ZipArchive, setMtimeName, arginfo_class_ZipArchive_setMtimeName, ZEND_ACC_PUBLIC)
#endif
	ZEND_ME(ZipArchive, getCommentIndex, arginfo_class_ZipArchive_getCommentIndex, ZEND_ACC_PUBLIC)
	ZEND_ME(ZipArchive, getCommentName, arginfo_class_ZipArchive_getCommentName, ZEND_ACC_PUBLIC)
	ZEND_ME(ZipArchive, deleteIndex, arginfo_class_ZipArchive_deleteIndex, ZEND_ACC_PUBLIC)
	ZEND_ME(ZipArchive, deleteName, arginfo_class_ZipArchive_deleteName, ZEND_ACC_PUBLIC)
	ZEND_ME(ZipArchive, statName, arginfo_class_ZipArchive_statName, ZEND_ACC_PUBLIC)
	ZEND_ME(ZipArchive, statIndex, arginfo_class_ZipArchive_statIndex, ZEND_ACC_PUBLIC)
	ZEND_ME(ZipArchive, locateName, arginfo_class_ZipArchive_locateName, ZEND_ACC_PUBLIC)
	ZEND_ME(ZipArchive, getNameIndex, arginfo_class_ZipArchive_getNameIndex, ZEND_ACC_PUBLIC)
	ZEND_ME(ZipArchive, unchangeArchive, arginfo_class_ZipArchive_unchangeArchive, ZEND_ACC_PUBLIC)
	ZEND_ME(ZipArchive, unchangeAll, arginfo_class_ZipArchive_unchangeAll, ZEND_ACC_PUBLIC)
	ZEND_ME(ZipArchive, unchangeIndex, arginfo_class_ZipArchive_unchangeIndex, ZEND_ACC_PUBLIC)
	ZEND_ME(ZipArchive, unchangeName, arginfo_class_ZipArchive_unchangeName, ZEND_ACC_PUBLIC)
	ZEND_ME(ZipArchive, extractTo, arginfo_class_ZipArchive_extractTo, ZEND_ACC_PUBLIC)
	ZEND_ME(ZipArchive, getFromName, arginfo_class_ZipArchive_getFromName, ZEND_ACC_PUBLIC)
	ZEND_ME(ZipArchive, getFromIndex, arginfo_class_ZipArchive_getFromIndex, ZEND_ACC_PUBLIC)
	ZEND_ME(ZipArchive, getStream, arginfo_class_ZipArchive_getStream, ZEND_ACC_PUBLIC)
#if defined(ZIP_OPSYS_DEFAULT)
	ZEND_ME(ZipArchive, setExternalAttributesName, arginfo_class_ZipArchive_setExternalAttributesName, ZEND_ACC_PUBLIC)
#endif
#if defined(ZIP_OPSYS_DEFAULT)
	ZEND_ME(ZipArchive, setExternalAttributesIndex, arginfo_class_ZipArchive_setExternalAttributesIndex, ZEND_ACC_PUBLIC)
#endif
#if defined(ZIP_OPSYS_DEFAULT)
	ZEND_ME(ZipArchive, getExternalAttributesName, arginfo_class_ZipArchive_getExternalAttributesName, ZEND_ACC_PUBLIC)
#endif
#if defined(ZIP_OPSYS_DEFAULT)
	ZEND_ME(ZipArchive, getExternalAttributesIndex, arginfo_class_ZipArchive_getExternalAttributesIndex, ZEND_ACC_PUBLIC)
#endif
	ZEND_ME(ZipArchive, setCompressionName, arginfo_class_ZipArchive_setCompressionName, ZEND_ACC_PUBLIC)
	ZEND_ME(ZipArchive, setCompressionIndex, arginfo_class_ZipArchive_setCompressionIndex, ZEND_ACC_PUBLIC)
#if defined(HAVE_ENCRYPTION)
	ZEND_ME(ZipArchive, setEncryptionName, arginfo_class_ZipArchive_setEncryptionName, ZEND_ACC_PUBLIC)
#endif
#if defined(HAVE_ENCRYPTION)
	ZEND_ME(ZipArchive, setEncryptionIndex, arginfo_class_ZipArchive_setEncryptionIndex, ZEND_ACC_PUBLIC)
#endif
#if defined(HAVE_PROGRESS_CALLBACK)
	ZEND_ME(ZipArchive, registerProgressCallback, arginfo_class_ZipArchive_registerProgressCallback, ZEND_ACC_PUBLIC)
#endif
#if defined(HAVE_CANCEL_CALLBACK)
	ZEND_ME(ZipArchive, registerCancelCallback, arginfo_class_ZipArchive_registerCancelCallback, ZEND_ACC_PUBLIC)
#endif
#if defined(HAVE_METHOD_SUPPORTED)
	ZEND_ME(ZipArchive, isCompressionMethodSupported, arginfo_class_ZipArchive_isCompressionMethodSupported, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
#endif
#if defined(HAVE_METHOD_SUPPORTED)
	ZEND_ME(ZipArchive, isEncryptionMethodSupported, arginfo_class_ZipArchive_isEncryptionMethodSupported, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
#endif
	ZEND_FE_END
};
