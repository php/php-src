/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 95564c667a51a548f5d43025c90546b991970ddd */

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

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_ZipArchive_open, 0, 1, MAY_BE_BOOL|MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ZipArchive_setPassword, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ZipArchive_close, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ZipArchive_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ZipArchive_getStatusString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ZipArchive_clearError, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ZipArchive_addEmptyDir, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, dirname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ZipArchive_addFromString, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, content, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "ZipArchive::FL_OVERWRITE")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ZipArchive_addFile, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filepath, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, entryname, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, start, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "ZipArchive::LENGTH_TO_END")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "ZipArchive::FL_OVERWRITE")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ZipArchive_replaceFile, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filepath, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, start, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "ZipArchive::LENGTH_TO_END")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_ZipArchive_addGlob, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_ZipArchive_addPattern, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, path, IS_STRING, 0, "\".\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ZipArchive_renameIndex, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, new_name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ZipArchive_renameName, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, new_name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ZipArchive_setArchiveComment, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, comment, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_ZipArchive_getArchiveComment, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ZipArchive_setArchiveFlag, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, flag, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ZipArchive_getArchiveFlag, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, flag, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ZipArchive_setCommentIndex, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, comment, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ZipArchive_setCommentName, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, comment, IS_STRING, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_SET_MTIME)
ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ZipArchive_setMtimeIndex, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, timestamp, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_SET_MTIME)
ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ZipArchive_setMtimeName, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, timestamp, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_ZipArchive_getCommentIndex, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_ZipArchive_getCommentName, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ZipArchive_deleteIndex, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ZipArchive_deleteName, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_ZipArchive_statName, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_ZipArchive_statIndex, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_ZipArchive_locateName, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_ZipArchive_getNameIndex arginfo_class_ZipArchive_getCommentIndex

#define arginfo_class_ZipArchive_unchangeArchive arginfo_class_ZipArchive_close

#define arginfo_class_ZipArchive_unchangeAll arginfo_class_ZipArchive_close

#define arginfo_class_ZipArchive_unchangeIndex arginfo_class_ZipArchive_deleteIndex

#define arginfo_class_ZipArchive_unchangeName arginfo_class_ZipArchive_deleteName

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ZipArchive_extractTo, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pathto, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, files, MAY_BE_ARRAY|MAY_BE_STRING|MAY_BE_NULL, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_ZipArchive_getFromName, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, len, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_ZipArchive_getFromIndex, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, len, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_getStreamIndex, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_getStreamName, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_getStream, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

#if defined(ZIP_OPSYS_DEFAULT)
ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ZipArchive_setExternalAttributesName, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, opsys, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, attr, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

#if defined(ZIP_OPSYS_DEFAULT)
ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ZipArchive_setExternalAttributesIndex, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, opsys, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, attr, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

#if defined(ZIP_OPSYS_DEFAULT)
ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ZipArchive_getExternalAttributesName, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_INFO(1, opsys)
	ZEND_ARG_INFO(1, attr)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

#if defined(ZIP_OPSYS_DEFAULT)
ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ZipArchive_getExternalAttributesIndex, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_INFO(1, opsys)
	ZEND_ARG_INFO(1, attr)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ZipArchive_setCompressionName, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, method, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, compflags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ZipArchive_setCompressionIndex, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, method, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, compflags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#if defined(HAVE_ENCRYPTION)
ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ZipArchive_setEncryptionName, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, method, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, password, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_ENCRYPTION)
ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ZipArchive_setEncryptionIndex, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, method, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, password, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_PROGRESS_CALLBACK)
ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ZipArchive_registerProgressCallback, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, rate, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_CANCEL_CALLBACK)
ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ZipArchive_registerCancelCallback, 0, 1, _IS_BOOL, 0)
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
ZEND_METHOD(ZipArchive, clearError);
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
ZEND_METHOD(ZipArchive, setArchiveFlag);
ZEND_METHOD(ZipArchive, getArchiveFlag);
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
ZEND_METHOD(ZipArchive, getStreamIndex);
ZEND_METHOD(ZipArchive, getStreamName);
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
	ZEND_ME(ZipArchive, clearError, arginfo_class_ZipArchive_clearError, ZEND_ACC_PUBLIC)
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
	ZEND_ME(ZipArchive, setArchiveFlag, arginfo_class_ZipArchive_setArchiveFlag, ZEND_ACC_PUBLIC)
	ZEND_ME(ZipArchive, getArchiveFlag, arginfo_class_ZipArchive_getArchiveFlag, ZEND_ACC_PUBLIC)
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
	ZEND_ME(ZipArchive, getStreamIndex, arginfo_class_ZipArchive_getStreamIndex, ZEND_ACC_PUBLIC)
	ZEND_ME(ZipArchive, getStreamName, arginfo_class_ZipArchive_getStreamName, ZEND_ACC_PUBLIC)
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

static zend_class_entry *register_class_ZipArchive(zend_class_entry *class_entry_Countable)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ZipArchive", class_ZipArchive_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	zend_class_implements(class_entry, 1, class_entry_Countable);

	zval const_CREATE_value;
	ZVAL_LONG(&const_CREATE_value, ZIP_CREATE);
	zend_string *const_CREATE_name = zend_string_init_interned("CREATE", sizeof("CREATE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CREATE_name, &const_CREATE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CREATE_name);

	zval const_EXCL_value;
	ZVAL_LONG(&const_EXCL_value, ZIP_EXCL);
	zend_string *const_EXCL_name = zend_string_init_interned("EXCL", sizeof("EXCL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_EXCL_name, &const_EXCL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_EXCL_name);

	zval const_CHECKCONS_value;
	ZVAL_LONG(&const_CHECKCONS_value, ZIP_CHECKCONS);
	zend_string *const_CHECKCONS_name = zend_string_init_interned("CHECKCONS", sizeof("CHECKCONS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHECKCONS_name, &const_CHECKCONS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHECKCONS_name);

	zval const_OVERWRITE_value;
	ZVAL_LONG(&const_OVERWRITE_value, ZIP_OVERWRITE);
	zend_string *const_OVERWRITE_name = zend_string_init_interned("OVERWRITE", sizeof("OVERWRITE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_OVERWRITE_name, &const_OVERWRITE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_OVERWRITE_name);
#if defined(ZIP_RDONLY)

	zval const_RDONLY_value;
	ZVAL_LONG(&const_RDONLY_value, ZIP_RDONLY);
	zend_string *const_RDONLY_name = zend_string_init_interned("RDONLY", sizeof("RDONLY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_RDONLY_name, &const_RDONLY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_RDONLY_name);
#endif

	zval const_FL_NOCASE_value;
	ZVAL_LONG(&const_FL_NOCASE_value, ZIP_FL_NOCASE);
	zend_string *const_FL_NOCASE_name = zend_string_init_interned("FL_NOCASE", sizeof("FL_NOCASE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FL_NOCASE_name, &const_FL_NOCASE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FL_NOCASE_name);

	zval const_FL_NODIR_value;
	ZVAL_LONG(&const_FL_NODIR_value, ZIP_FL_NODIR);
	zend_string *const_FL_NODIR_name = zend_string_init_interned("FL_NODIR", sizeof("FL_NODIR") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FL_NODIR_name, &const_FL_NODIR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FL_NODIR_name);

	zval const_FL_COMPRESSED_value;
	ZVAL_LONG(&const_FL_COMPRESSED_value, ZIP_FL_COMPRESSED);
	zend_string *const_FL_COMPRESSED_name = zend_string_init_interned("FL_COMPRESSED", sizeof("FL_COMPRESSED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FL_COMPRESSED_name, &const_FL_COMPRESSED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FL_COMPRESSED_name);

	zval const_FL_UNCHANGED_value;
	ZVAL_LONG(&const_FL_UNCHANGED_value, ZIP_FL_UNCHANGED);
	zend_string *const_FL_UNCHANGED_name = zend_string_init_interned("FL_UNCHANGED", sizeof("FL_UNCHANGED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FL_UNCHANGED_name, &const_FL_UNCHANGED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FL_UNCHANGED_name);
#if defined(ZIP_FL_RECOMPRESS)

	zval const_FL_RECOMPRESS_value;
	ZVAL_LONG(&const_FL_RECOMPRESS_value, ZIP_FL_RECOMPRESS);
	zend_string *const_FL_RECOMPRESS_name = zend_string_init_interned("FL_RECOMPRESS", sizeof("FL_RECOMPRESS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FL_RECOMPRESS_name, &const_FL_RECOMPRESS_value, ZEND_ACC_PUBLIC|ZEND_ACC_DEPRECATED, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FL_RECOMPRESS_name);
#endif

	zval const_FL_ENCRYPTED_value;
	ZVAL_LONG(&const_FL_ENCRYPTED_value, ZIP_FL_ENCRYPTED);
	zend_string *const_FL_ENCRYPTED_name = zend_string_init_interned("FL_ENCRYPTED", sizeof("FL_ENCRYPTED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FL_ENCRYPTED_name, &const_FL_ENCRYPTED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FL_ENCRYPTED_name);

	zval const_FL_OVERWRITE_value;
	ZVAL_LONG(&const_FL_OVERWRITE_value, ZIP_FL_OVERWRITE);
	zend_string *const_FL_OVERWRITE_name = zend_string_init_interned("FL_OVERWRITE", sizeof("FL_OVERWRITE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FL_OVERWRITE_name, &const_FL_OVERWRITE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FL_OVERWRITE_name);

	zval const_FL_LOCAL_value;
	ZVAL_LONG(&const_FL_LOCAL_value, ZIP_FL_LOCAL);
	zend_string *const_FL_LOCAL_name = zend_string_init_interned("FL_LOCAL", sizeof("FL_LOCAL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FL_LOCAL_name, &const_FL_LOCAL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FL_LOCAL_name);

	zval const_FL_CENTRAL_value;
	ZVAL_LONG(&const_FL_CENTRAL_value, ZIP_FL_CENTRAL);
	zend_string *const_FL_CENTRAL_name = zend_string_init_interned("FL_CENTRAL", sizeof("FL_CENTRAL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FL_CENTRAL_name, &const_FL_CENTRAL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FL_CENTRAL_name);

	zval const_FL_ENC_GUESS_value;
	ZVAL_LONG(&const_FL_ENC_GUESS_value, ZIP_FL_ENC_GUESS);
	zend_string *const_FL_ENC_GUESS_name = zend_string_init_interned("FL_ENC_GUESS", sizeof("FL_ENC_GUESS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FL_ENC_GUESS_name, &const_FL_ENC_GUESS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FL_ENC_GUESS_name);

	zval const_FL_ENC_RAW_value;
	ZVAL_LONG(&const_FL_ENC_RAW_value, ZIP_FL_ENC_RAW);
	zend_string *const_FL_ENC_RAW_name = zend_string_init_interned("FL_ENC_RAW", sizeof("FL_ENC_RAW") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FL_ENC_RAW_name, &const_FL_ENC_RAW_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FL_ENC_RAW_name);

	zval const_FL_ENC_STRICT_value;
	ZVAL_LONG(&const_FL_ENC_STRICT_value, ZIP_FL_ENC_STRICT);
	zend_string *const_FL_ENC_STRICT_name = zend_string_init_interned("FL_ENC_STRICT", sizeof("FL_ENC_STRICT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FL_ENC_STRICT_name, &const_FL_ENC_STRICT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FL_ENC_STRICT_name);

	zval const_FL_ENC_UTF_8_value;
	ZVAL_LONG(&const_FL_ENC_UTF_8_value, ZIP_FL_ENC_UTF_8);
	zend_string *const_FL_ENC_UTF_8_name = zend_string_init_interned("FL_ENC_UTF_8", sizeof("FL_ENC_UTF_8") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FL_ENC_UTF_8_name, &const_FL_ENC_UTF_8_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FL_ENC_UTF_8_name);

	zval const_FL_ENC_CP437_value;
	ZVAL_LONG(&const_FL_ENC_CP437_value, ZIP_FL_ENC_CP437);
	zend_string *const_FL_ENC_CP437_name = zend_string_init_interned("FL_ENC_CP437", sizeof("FL_ENC_CP437") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FL_ENC_CP437_name, &const_FL_ENC_CP437_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FL_ENC_CP437_name);

	zval const_FL_OPEN_FILE_NOW_value;
	ZVAL_LONG(&const_FL_OPEN_FILE_NOW_value, ZIP_FL_OPEN_FILE_NOW);
	zend_string *const_FL_OPEN_FILE_NOW_name = zend_string_init_interned("FL_OPEN_FILE_NOW", sizeof("FL_OPEN_FILE_NOW") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FL_OPEN_FILE_NOW_name, &const_FL_OPEN_FILE_NOW_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FL_OPEN_FILE_NOW_name);

	zval const_CM_DEFAULT_value;
	ZVAL_LONG(&const_CM_DEFAULT_value, ZIP_CM_DEFAULT);
	zend_string *const_CM_DEFAULT_name = zend_string_init_interned("CM_DEFAULT", sizeof("CM_DEFAULT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CM_DEFAULT_name, &const_CM_DEFAULT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CM_DEFAULT_name);

	zval const_CM_STORE_value;
	ZVAL_LONG(&const_CM_STORE_value, ZIP_CM_STORE);
	zend_string *const_CM_STORE_name = zend_string_init_interned("CM_STORE", sizeof("CM_STORE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CM_STORE_name, &const_CM_STORE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CM_STORE_name);

	zval const_CM_SHRINK_value;
	ZVAL_LONG(&const_CM_SHRINK_value, ZIP_CM_SHRINK);
	zend_string *const_CM_SHRINK_name = zend_string_init_interned("CM_SHRINK", sizeof("CM_SHRINK") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CM_SHRINK_name, &const_CM_SHRINK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CM_SHRINK_name);

	zval const_CM_REDUCE_1_value;
	ZVAL_LONG(&const_CM_REDUCE_1_value, ZIP_CM_REDUCE_1);
	zend_string *const_CM_REDUCE_1_name = zend_string_init_interned("CM_REDUCE_1", sizeof("CM_REDUCE_1") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CM_REDUCE_1_name, &const_CM_REDUCE_1_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CM_REDUCE_1_name);

	zval const_CM_REDUCE_2_value;
	ZVAL_LONG(&const_CM_REDUCE_2_value, ZIP_CM_REDUCE_2);
	zend_string *const_CM_REDUCE_2_name = zend_string_init_interned("CM_REDUCE_2", sizeof("CM_REDUCE_2") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CM_REDUCE_2_name, &const_CM_REDUCE_2_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CM_REDUCE_2_name);

	zval const_CM_REDUCE_3_value;
	ZVAL_LONG(&const_CM_REDUCE_3_value, ZIP_CM_REDUCE_3);
	zend_string *const_CM_REDUCE_3_name = zend_string_init_interned("CM_REDUCE_3", sizeof("CM_REDUCE_3") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CM_REDUCE_3_name, &const_CM_REDUCE_3_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CM_REDUCE_3_name);

	zval const_CM_REDUCE_4_value;
	ZVAL_LONG(&const_CM_REDUCE_4_value, ZIP_CM_REDUCE_4);
	zend_string *const_CM_REDUCE_4_name = zend_string_init_interned("CM_REDUCE_4", sizeof("CM_REDUCE_4") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CM_REDUCE_4_name, &const_CM_REDUCE_4_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CM_REDUCE_4_name);

	zval const_CM_IMPLODE_value;
	ZVAL_LONG(&const_CM_IMPLODE_value, ZIP_CM_IMPLODE);
	zend_string *const_CM_IMPLODE_name = zend_string_init_interned("CM_IMPLODE", sizeof("CM_IMPLODE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CM_IMPLODE_name, &const_CM_IMPLODE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CM_IMPLODE_name);

	zval const_CM_DEFLATE_value;
	ZVAL_LONG(&const_CM_DEFLATE_value, ZIP_CM_DEFLATE);
	zend_string *const_CM_DEFLATE_name = zend_string_init_interned("CM_DEFLATE", sizeof("CM_DEFLATE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CM_DEFLATE_name, &const_CM_DEFLATE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CM_DEFLATE_name);

	zval const_CM_DEFLATE64_value;
	ZVAL_LONG(&const_CM_DEFLATE64_value, ZIP_CM_DEFLATE64);
	zend_string *const_CM_DEFLATE64_name = zend_string_init_interned("CM_DEFLATE64", sizeof("CM_DEFLATE64") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CM_DEFLATE64_name, &const_CM_DEFLATE64_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CM_DEFLATE64_name);

	zval const_CM_PKWARE_IMPLODE_value;
	ZVAL_LONG(&const_CM_PKWARE_IMPLODE_value, ZIP_CM_PKWARE_IMPLODE);
	zend_string *const_CM_PKWARE_IMPLODE_name = zend_string_init_interned("CM_PKWARE_IMPLODE", sizeof("CM_PKWARE_IMPLODE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CM_PKWARE_IMPLODE_name, &const_CM_PKWARE_IMPLODE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CM_PKWARE_IMPLODE_name);

	zval const_CM_BZIP2_value;
	ZVAL_LONG(&const_CM_BZIP2_value, ZIP_CM_BZIP2);
	zend_string *const_CM_BZIP2_name = zend_string_init_interned("CM_BZIP2", sizeof("CM_BZIP2") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CM_BZIP2_name, &const_CM_BZIP2_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CM_BZIP2_name);

	zval const_CM_LZMA_value;
	ZVAL_LONG(&const_CM_LZMA_value, ZIP_CM_LZMA);
	zend_string *const_CM_LZMA_name = zend_string_init_interned("CM_LZMA", sizeof("CM_LZMA") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CM_LZMA_name, &const_CM_LZMA_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CM_LZMA_name);
#if defined(ZIP_CM_LZMA2)

	zval const_CM_LZMA2_value;
	ZVAL_LONG(&const_CM_LZMA2_value, ZIP_CM_LZMA2);
	zend_string *const_CM_LZMA2_name = zend_string_init_interned("CM_LZMA2", sizeof("CM_LZMA2") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CM_LZMA2_name, &const_CM_LZMA2_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CM_LZMA2_name);
#endif
#if defined(ZIP_CM_ZSTD)

	zval const_CM_ZSTD_value;
	ZVAL_LONG(&const_CM_ZSTD_value, ZIP_CM_ZSTD);
	zend_string *const_CM_ZSTD_name = zend_string_init_interned("CM_ZSTD", sizeof("CM_ZSTD") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CM_ZSTD_name, &const_CM_ZSTD_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CM_ZSTD_name);
#endif
#if defined(ZIP_CM_XZ)

	zval const_CM_XZ_value;
	ZVAL_LONG(&const_CM_XZ_value, ZIP_CM_XZ);
	zend_string *const_CM_XZ_name = zend_string_init_interned("CM_XZ", sizeof("CM_XZ") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CM_XZ_name, &const_CM_XZ_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CM_XZ_name);
#endif

	zval const_CM_TERSE_value;
	ZVAL_LONG(&const_CM_TERSE_value, ZIP_CM_TERSE);
	zend_string *const_CM_TERSE_name = zend_string_init_interned("CM_TERSE", sizeof("CM_TERSE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CM_TERSE_name, &const_CM_TERSE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CM_TERSE_name);

	zval const_CM_LZ77_value;
	ZVAL_LONG(&const_CM_LZ77_value, ZIP_CM_LZ77);
	zend_string *const_CM_LZ77_name = zend_string_init_interned("CM_LZ77", sizeof("CM_LZ77") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CM_LZ77_name, &const_CM_LZ77_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CM_LZ77_name);

	zval const_CM_WAVPACK_value;
	ZVAL_LONG(&const_CM_WAVPACK_value, ZIP_CM_WAVPACK);
	zend_string *const_CM_WAVPACK_name = zend_string_init_interned("CM_WAVPACK", sizeof("CM_WAVPACK") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CM_WAVPACK_name, &const_CM_WAVPACK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CM_WAVPACK_name);

	zval const_CM_PPMD_value;
	ZVAL_LONG(&const_CM_PPMD_value, ZIP_CM_PPMD);
	zend_string *const_CM_PPMD_name = zend_string_init_interned("CM_PPMD", sizeof("CM_PPMD") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CM_PPMD_name, &const_CM_PPMD_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CM_PPMD_name);

	zval const_ER_OK_value;
	ZVAL_LONG(&const_ER_OK_value, ZIP_ER_OK);
	zend_string *const_ER_OK_name = zend_string_init_interned("ER_OK", sizeof("ER_OK") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ER_OK_name, &const_ER_OK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ER_OK_name);

	zval const_ER_MULTIDISK_value;
	ZVAL_LONG(&const_ER_MULTIDISK_value, ZIP_ER_MULTIDISK);
	zend_string *const_ER_MULTIDISK_name = zend_string_init_interned("ER_MULTIDISK", sizeof("ER_MULTIDISK") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ER_MULTIDISK_name, &const_ER_MULTIDISK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ER_MULTIDISK_name);

	zval const_ER_RENAME_value;
	ZVAL_LONG(&const_ER_RENAME_value, ZIP_ER_RENAME);
	zend_string *const_ER_RENAME_name = zend_string_init_interned("ER_RENAME", sizeof("ER_RENAME") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ER_RENAME_name, &const_ER_RENAME_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ER_RENAME_name);

	zval const_ER_CLOSE_value;
	ZVAL_LONG(&const_ER_CLOSE_value, ZIP_ER_CLOSE);
	zend_string *const_ER_CLOSE_name = zend_string_init_interned("ER_CLOSE", sizeof("ER_CLOSE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ER_CLOSE_name, &const_ER_CLOSE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ER_CLOSE_name);

	zval const_ER_SEEK_value;
	ZVAL_LONG(&const_ER_SEEK_value, ZIP_ER_SEEK);
	zend_string *const_ER_SEEK_name = zend_string_init_interned("ER_SEEK", sizeof("ER_SEEK") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ER_SEEK_name, &const_ER_SEEK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ER_SEEK_name);

	zval const_ER_READ_value;
	ZVAL_LONG(&const_ER_READ_value, ZIP_ER_READ);
	zend_string *const_ER_READ_name = zend_string_init_interned("ER_READ", sizeof("ER_READ") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ER_READ_name, &const_ER_READ_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ER_READ_name);

	zval const_ER_WRITE_value;
	ZVAL_LONG(&const_ER_WRITE_value, ZIP_ER_WRITE);
	zend_string *const_ER_WRITE_name = zend_string_init_interned("ER_WRITE", sizeof("ER_WRITE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ER_WRITE_name, &const_ER_WRITE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ER_WRITE_name);

	zval const_ER_CRC_value;
	ZVAL_LONG(&const_ER_CRC_value, ZIP_ER_CRC);
	zend_string *const_ER_CRC_name = zend_string_init_interned("ER_CRC", sizeof("ER_CRC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ER_CRC_name, &const_ER_CRC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ER_CRC_name);

	zval const_ER_ZIPCLOSED_value;
	ZVAL_LONG(&const_ER_ZIPCLOSED_value, ZIP_ER_ZIPCLOSED);
	zend_string *const_ER_ZIPCLOSED_name = zend_string_init_interned("ER_ZIPCLOSED", sizeof("ER_ZIPCLOSED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ER_ZIPCLOSED_name, &const_ER_ZIPCLOSED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ER_ZIPCLOSED_name);

	zval const_ER_NOENT_value;
	ZVAL_LONG(&const_ER_NOENT_value, ZIP_ER_NOENT);
	zend_string *const_ER_NOENT_name = zend_string_init_interned("ER_NOENT", sizeof("ER_NOENT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ER_NOENT_name, &const_ER_NOENT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ER_NOENT_name);

	zval const_ER_EXISTS_value;
	ZVAL_LONG(&const_ER_EXISTS_value, ZIP_ER_EXISTS);
	zend_string *const_ER_EXISTS_name = zend_string_init_interned("ER_EXISTS", sizeof("ER_EXISTS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ER_EXISTS_name, &const_ER_EXISTS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ER_EXISTS_name);

	zval const_ER_OPEN_value;
	ZVAL_LONG(&const_ER_OPEN_value, ZIP_ER_OPEN);
	zend_string *const_ER_OPEN_name = zend_string_init_interned("ER_OPEN", sizeof("ER_OPEN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ER_OPEN_name, &const_ER_OPEN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ER_OPEN_name);

	zval const_ER_TMPOPEN_value;
	ZVAL_LONG(&const_ER_TMPOPEN_value, ZIP_ER_TMPOPEN);
	zend_string *const_ER_TMPOPEN_name = zend_string_init_interned("ER_TMPOPEN", sizeof("ER_TMPOPEN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ER_TMPOPEN_name, &const_ER_TMPOPEN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ER_TMPOPEN_name);

	zval const_ER_ZLIB_value;
	ZVAL_LONG(&const_ER_ZLIB_value, ZIP_ER_ZLIB);
	zend_string *const_ER_ZLIB_name = zend_string_init_interned("ER_ZLIB", sizeof("ER_ZLIB") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ER_ZLIB_name, &const_ER_ZLIB_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ER_ZLIB_name);

	zval const_ER_MEMORY_value;
	ZVAL_LONG(&const_ER_MEMORY_value, ZIP_ER_MEMORY);
	zend_string *const_ER_MEMORY_name = zend_string_init_interned("ER_MEMORY", sizeof("ER_MEMORY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ER_MEMORY_name, &const_ER_MEMORY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ER_MEMORY_name);

	zval const_ER_CHANGED_value;
	ZVAL_LONG(&const_ER_CHANGED_value, ZIP_ER_CHANGED);
	zend_string *const_ER_CHANGED_name = zend_string_init_interned("ER_CHANGED", sizeof("ER_CHANGED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ER_CHANGED_name, &const_ER_CHANGED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ER_CHANGED_name);

	zval const_ER_COMPNOTSUPP_value;
	ZVAL_LONG(&const_ER_COMPNOTSUPP_value, ZIP_ER_COMPNOTSUPP);
	zend_string *const_ER_COMPNOTSUPP_name = zend_string_init_interned("ER_COMPNOTSUPP", sizeof("ER_COMPNOTSUPP") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ER_COMPNOTSUPP_name, &const_ER_COMPNOTSUPP_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ER_COMPNOTSUPP_name);

	zval const_ER_EOF_value;
	ZVAL_LONG(&const_ER_EOF_value, ZIP_ER_EOF);
	zend_string *const_ER_EOF_name = zend_string_init_interned("ER_EOF", sizeof("ER_EOF") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ER_EOF_name, &const_ER_EOF_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ER_EOF_name);

	zval const_ER_INVAL_value;
	ZVAL_LONG(&const_ER_INVAL_value, ZIP_ER_INVAL);
	zend_string *const_ER_INVAL_name = zend_string_init_interned("ER_INVAL", sizeof("ER_INVAL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ER_INVAL_name, &const_ER_INVAL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ER_INVAL_name);

	zval const_ER_NOZIP_value;
	ZVAL_LONG(&const_ER_NOZIP_value, ZIP_ER_NOZIP);
	zend_string *const_ER_NOZIP_name = zend_string_init_interned("ER_NOZIP", sizeof("ER_NOZIP") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ER_NOZIP_name, &const_ER_NOZIP_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ER_NOZIP_name);

	zval const_ER_INTERNAL_value;
	ZVAL_LONG(&const_ER_INTERNAL_value, ZIP_ER_INTERNAL);
	zend_string *const_ER_INTERNAL_name = zend_string_init_interned("ER_INTERNAL", sizeof("ER_INTERNAL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ER_INTERNAL_name, &const_ER_INTERNAL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ER_INTERNAL_name);

	zval const_ER_INCONS_value;
	ZVAL_LONG(&const_ER_INCONS_value, ZIP_ER_INCONS);
	zend_string *const_ER_INCONS_name = zend_string_init_interned("ER_INCONS", sizeof("ER_INCONS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ER_INCONS_name, &const_ER_INCONS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ER_INCONS_name);

	zval const_ER_REMOVE_value;
	ZVAL_LONG(&const_ER_REMOVE_value, ZIP_ER_REMOVE);
	zend_string *const_ER_REMOVE_name = zend_string_init_interned("ER_REMOVE", sizeof("ER_REMOVE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ER_REMOVE_name, &const_ER_REMOVE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ER_REMOVE_name);

	zval const_ER_DELETED_value;
	ZVAL_LONG(&const_ER_DELETED_value, ZIP_ER_DELETED);
	zend_string *const_ER_DELETED_name = zend_string_init_interned("ER_DELETED", sizeof("ER_DELETED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ER_DELETED_name, &const_ER_DELETED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ER_DELETED_name);

	zval const_ER_ENCRNOTSUPP_value;
	ZVAL_LONG(&const_ER_ENCRNOTSUPP_value, ZIP_ER_ENCRNOTSUPP);
	zend_string *const_ER_ENCRNOTSUPP_name = zend_string_init_interned("ER_ENCRNOTSUPP", sizeof("ER_ENCRNOTSUPP") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ER_ENCRNOTSUPP_name, &const_ER_ENCRNOTSUPP_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ER_ENCRNOTSUPP_name);

	zval const_ER_RDONLY_value;
	ZVAL_LONG(&const_ER_RDONLY_value, ZIP_ER_RDONLY);
	zend_string *const_ER_RDONLY_name = zend_string_init_interned("ER_RDONLY", sizeof("ER_RDONLY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ER_RDONLY_name, &const_ER_RDONLY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ER_RDONLY_name);

	zval const_ER_NOPASSWD_value;
	ZVAL_LONG(&const_ER_NOPASSWD_value, ZIP_ER_NOPASSWD);
	zend_string *const_ER_NOPASSWD_name = zend_string_init_interned("ER_NOPASSWD", sizeof("ER_NOPASSWD") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ER_NOPASSWD_name, &const_ER_NOPASSWD_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ER_NOPASSWD_name);

	zval const_ER_WRONGPASSWD_value;
	ZVAL_LONG(&const_ER_WRONGPASSWD_value, ZIP_ER_WRONGPASSWD);
	zend_string *const_ER_WRONGPASSWD_name = zend_string_init_interned("ER_WRONGPASSWD", sizeof("ER_WRONGPASSWD") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ER_WRONGPASSWD_name, &const_ER_WRONGPASSWD_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ER_WRONGPASSWD_name);
#if defined(ZIP_ER_OPNOTSUPP)

	zval const_ER_OPNOTSUPP_value;
	ZVAL_LONG(&const_ER_OPNOTSUPP_value, ZIP_ER_OPNOTSUPP);
	zend_string *const_ER_OPNOTSUPP_name = zend_string_init_interned("ER_OPNOTSUPP", sizeof("ER_OPNOTSUPP") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ER_OPNOTSUPP_name, &const_ER_OPNOTSUPP_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ER_OPNOTSUPP_name);
#endif
#if defined(ZIP_ER_INUSE)

	zval const_ER_INUSE_value;
	ZVAL_LONG(&const_ER_INUSE_value, ZIP_ER_INUSE);
	zend_string *const_ER_INUSE_name = zend_string_init_interned("ER_INUSE", sizeof("ER_INUSE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ER_INUSE_name, &const_ER_INUSE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ER_INUSE_name);
#endif
#if defined(ZIP_ER_TELL)

	zval const_ER_TELL_value;
	ZVAL_LONG(&const_ER_TELL_value, ZIP_ER_TELL);
	zend_string *const_ER_TELL_name = zend_string_init_interned("ER_TELL", sizeof("ER_TELL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ER_TELL_name, &const_ER_TELL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ER_TELL_name);
#endif
#if defined(ZIP_ER_COMPRESSED_DATA)

	zval const_ER_COMPRESSED_DATA_value;
	ZVAL_LONG(&const_ER_COMPRESSED_DATA_value, ZIP_ER_COMPRESSED_DATA);
	zend_string *const_ER_COMPRESSED_DATA_name = zend_string_init_interned("ER_COMPRESSED_DATA", sizeof("ER_COMPRESSED_DATA") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ER_COMPRESSED_DATA_name, &const_ER_COMPRESSED_DATA_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ER_COMPRESSED_DATA_name);
#endif
#if defined(ZIP_ER_CANCELLED)

	zval const_ER_CANCELLED_value;
	ZVAL_LONG(&const_ER_CANCELLED_value, ZIP_ER_CANCELLED);
	zend_string *const_ER_CANCELLED_name = zend_string_init_interned("ER_CANCELLED", sizeof("ER_CANCELLED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ER_CANCELLED_name, &const_ER_CANCELLED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ER_CANCELLED_name);
#endif
#if defined(ZIP_ER_DATA_LENGTH)

	zval const_ER_DATA_LENGTH_value;
	ZVAL_LONG(&const_ER_DATA_LENGTH_value, ZIP_ER_DATA_LENGTH);
	zend_string *const_ER_DATA_LENGTH_name = zend_string_init_interned("ER_DATA_LENGTH", sizeof("ER_DATA_LENGTH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ER_DATA_LENGTH_name, &const_ER_DATA_LENGTH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ER_DATA_LENGTH_name);
#endif
#if defined(ZIP_ER_NOT_ALLOWED)

	zval const_ER_NOT_ALLOWED_value;
	ZVAL_LONG(&const_ER_NOT_ALLOWED_value, ZIP_ER_NOT_ALLOWED);
	zend_string *const_ER_NOT_ALLOWED_name = zend_string_init_interned("ER_NOT_ALLOWED", sizeof("ER_NOT_ALLOWED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ER_NOT_ALLOWED_name, &const_ER_NOT_ALLOWED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ER_NOT_ALLOWED_name);
#endif
#if defined(ZIP_AFL_RDONLY)

	zval const_AFL_RDONLY_value;
	ZVAL_LONG(&const_AFL_RDONLY_value, ZIP_AFL_RDONLY);
	zend_string *const_AFL_RDONLY_name = zend_string_init_interned("AFL_RDONLY", sizeof("AFL_RDONLY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_AFL_RDONLY_name, &const_AFL_RDONLY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_AFL_RDONLY_name);
#endif
#if defined(ZIP_AFL_IS_TORRENTZIP)

	zval const_AFL_IS_TORRENTZIP_value;
	ZVAL_LONG(&const_AFL_IS_TORRENTZIP_value, ZIP_AFL_IS_TORRENTZIP);
	zend_string *const_AFL_IS_TORRENTZIP_name = zend_string_init_interned("AFL_IS_TORRENTZIP", sizeof("AFL_IS_TORRENTZIP") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_AFL_IS_TORRENTZIP_name, &const_AFL_IS_TORRENTZIP_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_AFL_IS_TORRENTZIP_name);
#endif
#if defined(ZIP_AFL_WANT_TORRENTZIP)

	zval const_AFL_WANT_TORRENTZIP_value;
	ZVAL_LONG(&const_AFL_WANT_TORRENTZIP_value, ZIP_AFL_WANT_TORRENTZIP);
	zend_string *const_AFL_WANT_TORRENTZIP_name = zend_string_init_interned("AFL_WANT_TORRENTZIP", sizeof("AFL_WANT_TORRENTZIP") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_AFL_WANT_TORRENTZIP_name, &const_AFL_WANT_TORRENTZIP_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_AFL_WANT_TORRENTZIP_name);
#endif
#if defined(ZIP_AFL_CREATE_OR_KEEP_FILE_FOR_EMPTY_ARCHIVE)

	zval const_AFL_CREATE_OR_KEEP_FILE_FOR_EMPTY_ARCHIVE_value;
	ZVAL_LONG(&const_AFL_CREATE_OR_KEEP_FILE_FOR_EMPTY_ARCHIVE_value, ZIP_AFL_CREATE_OR_KEEP_FILE_FOR_EMPTY_ARCHIVE);
	zend_string *const_AFL_CREATE_OR_KEEP_FILE_FOR_EMPTY_ARCHIVE_name = zend_string_init_interned("AFL_CREATE_OR_KEEP_FILE_FOR_EMPTY_ARCHIVE", sizeof("AFL_CREATE_OR_KEEP_FILE_FOR_EMPTY_ARCHIVE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_AFL_CREATE_OR_KEEP_FILE_FOR_EMPTY_ARCHIVE_name, &const_AFL_CREATE_OR_KEEP_FILE_FOR_EMPTY_ARCHIVE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_AFL_CREATE_OR_KEEP_FILE_FOR_EMPTY_ARCHIVE_name);
#endif
#if defined(ZIP_OPSYS_DEFAULT)

	zval const_OPSYS_DOS_value;
	ZVAL_LONG(&const_OPSYS_DOS_value, ZIP_OPSYS_DOS);
	zend_string *const_OPSYS_DOS_name = zend_string_init_interned("OPSYS_DOS", sizeof("OPSYS_DOS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_OPSYS_DOS_name, &const_OPSYS_DOS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_OPSYS_DOS_name);
#endif
#if defined(ZIP_OPSYS_DEFAULT)

	zval const_OPSYS_AMIGA_value;
	ZVAL_LONG(&const_OPSYS_AMIGA_value, ZIP_OPSYS_AMIGA);
	zend_string *const_OPSYS_AMIGA_name = zend_string_init_interned("OPSYS_AMIGA", sizeof("OPSYS_AMIGA") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_OPSYS_AMIGA_name, &const_OPSYS_AMIGA_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_OPSYS_AMIGA_name);
#endif
#if defined(ZIP_OPSYS_DEFAULT)

	zval const_OPSYS_OPENVMS_value;
	ZVAL_LONG(&const_OPSYS_OPENVMS_value, ZIP_OPSYS_OPENVMS);
	zend_string *const_OPSYS_OPENVMS_name = zend_string_init_interned("OPSYS_OPENVMS", sizeof("OPSYS_OPENVMS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_OPSYS_OPENVMS_name, &const_OPSYS_OPENVMS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_OPSYS_OPENVMS_name);
#endif
#if defined(ZIP_OPSYS_DEFAULT)

	zval const_OPSYS_UNIX_value;
	ZVAL_LONG(&const_OPSYS_UNIX_value, ZIP_OPSYS_UNIX);
	zend_string *const_OPSYS_UNIX_name = zend_string_init_interned("OPSYS_UNIX", sizeof("OPSYS_UNIX") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_OPSYS_UNIX_name, &const_OPSYS_UNIX_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_OPSYS_UNIX_name);
#endif
#if defined(ZIP_OPSYS_DEFAULT)

	zval const_OPSYS_VM_CMS_value;
	ZVAL_LONG(&const_OPSYS_VM_CMS_value, ZIP_OPSYS_VM_CMS);
	zend_string *const_OPSYS_VM_CMS_name = zend_string_init_interned("OPSYS_VM_CMS", sizeof("OPSYS_VM_CMS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_OPSYS_VM_CMS_name, &const_OPSYS_VM_CMS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_OPSYS_VM_CMS_name);
#endif
#if defined(ZIP_OPSYS_DEFAULT)

	zval const_OPSYS_ATARI_ST_value;
	ZVAL_LONG(&const_OPSYS_ATARI_ST_value, ZIP_OPSYS_ATARI_ST);
	zend_string *const_OPSYS_ATARI_ST_name = zend_string_init_interned("OPSYS_ATARI_ST", sizeof("OPSYS_ATARI_ST") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_OPSYS_ATARI_ST_name, &const_OPSYS_ATARI_ST_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_OPSYS_ATARI_ST_name);
#endif
#if defined(ZIP_OPSYS_DEFAULT)

	zval const_OPSYS_OS_2_value;
	ZVAL_LONG(&const_OPSYS_OS_2_value, ZIP_OPSYS_OS_2);
	zend_string *const_OPSYS_OS_2_name = zend_string_init_interned("OPSYS_OS_2", sizeof("OPSYS_OS_2") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_OPSYS_OS_2_name, &const_OPSYS_OS_2_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_OPSYS_OS_2_name);
#endif
#if defined(ZIP_OPSYS_DEFAULT)

	zval const_OPSYS_MACINTOSH_value;
	ZVAL_LONG(&const_OPSYS_MACINTOSH_value, ZIP_OPSYS_MACINTOSH);
	zend_string *const_OPSYS_MACINTOSH_name = zend_string_init_interned("OPSYS_MACINTOSH", sizeof("OPSYS_MACINTOSH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_OPSYS_MACINTOSH_name, &const_OPSYS_MACINTOSH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_OPSYS_MACINTOSH_name);
#endif
#if defined(ZIP_OPSYS_DEFAULT)

	zval const_OPSYS_Z_SYSTEM_value;
	ZVAL_LONG(&const_OPSYS_Z_SYSTEM_value, ZIP_OPSYS_Z_SYSTEM);
	zend_string *const_OPSYS_Z_SYSTEM_name = zend_string_init_interned("OPSYS_Z_SYSTEM", sizeof("OPSYS_Z_SYSTEM") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_OPSYS_Z_SYSTEM_name, &const_OPSYS_Z_SYSTEM_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_OPSYS_Z_SYSTEM_name);
#endif
#if defined(ZIP_OPSYS_DEFAULT)

	zval const_OPSYS_CPM_value;
	ZVAL_LONG(&const_OPSYS_CPM_value, ZIP_OPSYS_CPM);
	zend_string *const_OPSYS_CPM_name = zend_string_init_interned("OPSYS_CPM", sizeof("OPSYS_CPM") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_OPSYS_CPM_name, &const_OPSYS_CPM_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_OPSYS_CPM_name);
#endif
#if defined(ZIP_OPSYS_DEFAULT)

	zval const_OPSYS_WINDOWS_NTFS_value;
	ZVAL_LONG(&const_OPSYS_WINDOWS_NTFS_value, ZIP_OPSYS_WINDOWS_NTFS);
	zend_string *const_OPSYS_WINDOWS_NTFS_name = zend_string_init_interned("OPSYS_WINDOWS_NTFS", sizeof("OPSYS_WINDOWS_NTFS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_OPSYS_WINDOWS_NTFS_name, &const_OPSYS_WINDOWS_NTFS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_OPSYS_WINDOWS_NTFS_name);
#endif
#if defined(ZIP_OPSYS_DEFAULT)

	zval const_OPSYS_MVS_value;
	ZVAL_LONG(&const_OPSYS_MVS_value, ZIP_OPSYS_MVS);
	zend_string *const_OPSYS_MVS_name = zend_string_init_interned("OPSYS_MVS", sizeof("OPSYS_MVS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_OPSYS_MVS_name, &const_OPSYS_MVS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_OPSYS_MVS_name);
#endif
#if defined(ZIP_OPSYS_DEFAULT)

	zval const_OPSYS_VSE_value;
	ZVAL_LONG(&const_OPSYS_VSE_value, ZIP_OPSYS_VSE);
	zend_string *const_OPSYS_VSE_name = zend_string_init_interned("OPSYS_VSE", sizeof("OPSYS_VSE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_OPSYS_VSE_name, &const_OPSYS_VSE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_OPSYS_VSE_name);
#endif
#if defined(ZIP_OPSYS_DEFAULT)

	zval const_OPSYS_ACORN_RISC_value;
	ZVAL_LONG(&const_OPSYS_ACORN_RISC_value, ZIP_OPSYS_ACORN_RISC);
	zend_string *const_OPSYS_ACORN_RISC_name = zend_string_init_interned("OPSYS_ACORN_RISC", sizeof("OPSYS_ACORN_RISC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_OPSYS_ACORN_RISC_name, &const_OPSYS_ACORN_RISC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_OPSYS_ACORN_RISC_name);
#endif
#if defined(ZIP_OPSYS_DEFAULT)

	zval const_OPSYS_VFAT_value;
	ZVAL_LONG(&const_OPSYS_VFAT_value, ZIP_OPSYS_VFAT);
	zend_string *const_OPSYS_VFAT_name = zend_string_init_interned("OPSYS_VFAT", sizeof("OPSYS_VFAT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_OPSYS_VFAT_name, &const_OPSYS_VFAT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_OPSYS_VFAT_name);
#endif
#if defined(ZIP_OPSYS_DEFAULT)

	zval const_OPSYS_ALTERNATE_MVS_value;
	ZVAL_LONG(&const_OPSYS_ALTERNATE_MVS_value, ZIP_OPSYS_ALTERNATE_MVS);
	zend_string *const_OPSYS_ALTERNATE_MVS_name = zend_string_init_interned("OPSYS_ALTERNATE_MVS", sizeof("OPSYS_ALTERNATE_MVS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_OPSYS_ALTERNATE_MVS_name, &const_OPSYS_ALTERNATE_MVS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_OPSYS_ALTERNATE_MVS_name);
#endif
#if defined(ZIP_OPSYS_DEFAULT)

	zval const_OPSYS_BEOS_value;
	ZVAL_LONG(&const_OPSYS_BEOS_value, ZIP_OPSYS_BEOS);
	zend_string *const_OPSYS_BEOS_name = zend_string_init_interned("OPSYS_BEOS", sizeof("OPSYS_BEOS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_OPSYS_BEOS_name, &const_OPSYS_BEOS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_OPSYS_BEOS_name);
#endif
#if defined(ZIP_OPSYS_DEFAULT)

	zval const_OPSYS_TANDEM_value;
	ZVAL_LONG(&const_OPSYS_TANDEM_value, ZIP_OPSYS_TANDEM);
	zend_string *const_OPSYS_TANDEM_name = zend_string_init_interned("OPSYS_TANDEM", sizeof("OPSYS_TANDEM") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_OPSYS_TANDEM_name, &const_OPSYS_TANDEM_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_OPSYS_TANDEM_name);
#endif
#if defined(ZIP_OPSYS_DEFAULT)

	zval const_OPSYS_OS_400_value;
	ZVAL_LONG(&const_OPSYS_OS_400_value, ZIP_OPSYS_OS_400);
	zend_string *const_OPSYS_OS_400_name = zend_string_init_interned("OPSYS_OS_400", sizeof("OPSYS_OS_400") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_OPSYS_OS_400_name, &const_OPSYS_OS_400_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_OPSYS_OS_400_name);
#endif
#if defined(ZIP_OPSYS_DEFAULT)

	zval const_OPSYS_OS_X_value;
	ZVAL_LONG(&const_OPSYS_OS_X_value, ZIP_OPSYS_OS_X);
	zend_string *const_OPSYS_OS_X_name = zend_string_init_interned("OPSYS_OS_X", sizeof("OPSYS_OS_X") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_OPSYS_OS_X_name, &const_OPSYS_OS_X_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_OPSYS_OS_X_name);
#endif
#if defined(ZIP_OPSYS_DEFAULT)

	zval const_OPSYS_DEFAULT_value;
	ZVAL_LONG(&const_OPSYS_DEFAULT_value, ZIP_OPSYS_DEFAULT);
	zend_string *const_OPSYS_DEFAULT_name = zend_string_init_interned("OPSYS_DEFAULT", sizeof("OPSYS_DEFAULT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_OPSYS_DEFAULT_name, &const_OPSYS_DEFAULT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_OPSYS_DEFAULT_name);
#endif

	zval const_EM_NONE_value;
	ZVAL_LONG(&const_EM_NONE_value, ZIP_EM_NONE);
	zend_string *const_EM_NONE_name = zend_string_init_interned("EM_NONE", sizeof("EM_NONE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_EM_NONE_name, &const_EM_NONE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_EM_NONE_name);

	zval const_EM_TRAD_PKWARE_value;
	ZVAL_LONG(&const_EM_TRAD_PKWARE_value, ZIP_EM_TRAD_PKWARE);
	zend_string *const_EM_TRAD_PKWARE_name = zend_string_init_interned("EM_TRAD_PKWARE", sizeof("EM_TRAD_PKWARE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_EM_TRAD_PKWARE_name, &const_EM_TRAD_PKWARE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_EM_TRAD_PKWARE_name);
#if defined(HAVE_ENCRYPTION)

	zval const_EM_AES_128_value;
	ZVAL_LONG(&const_EM_AES_128_value, ZIP_EM_AES_128);
	zend_string *const_EM_AES_128_name = zend_string_init_interned("EM_AES_128", sizeof("EM_AES_128") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_EM_AES_128_name, &const_EM_AES_128_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_EM_AES_128_name);
#endif
#if defined(HAVE_ENCRYPTION)

	zval const_EM_AES_192_value;
	ZVAL_LONG(&const_EM_AES_192_value, ZIP_EM_AES_192);
	zend_string *const_EM_AES_192_name = zend_string_init_interned("EM_AES_192", sizeof("EM_AES_192") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_EM_AES_192_name, &const_EM_AES_192_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_EM_AES_192_name);
#endif
#if defined(HAVE_ENCRYPTION)

	zval const_EM_AES_256_value;
	ZVAL_LONG(&const_EM_AES_256_value, ZIP_EM_AES_256);
	zend_string *const_EM_AES_256_name = zend_string_init_interned("EM_AES_256", sizeof("EM_AES_256") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_EM_AES_256_name, &const_EM_AES_256_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_EM_AES_256_name);
#endif

	zval const_EM_UNKNOWN_value;
	ZVAL_LONG(&const_EM_UNKNOWN_value, ZIP_EM_UNKNOWN);
	zend_string *const_EM_UNKNOWN_name = zend_string_init_interned("EM_UNKNOWN", sizeof("EM_UNKNOWN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_EM_UNKNOWN_name, &const_EM_UNKNOWN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_EM_UNKNOWN_name);

	zval const_LIBZIP_VERSION_value;
	zend_string *const_LIBZIP_VERSION_value_str = zend_string_init(LIBZIP_VERSION_STR, strlen(LIBZIP_VERSION_STR), 1);
	ZVAL_STR(&const_LIBZIP_VERSION_value, const_LIBZIP_VERSION_value_str);
	zend_string *const_LIBZIP_VERSION_name = zend_string_init_interned("LIBZIP_VERSION", sizeof("LIBZIP_VERSION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LIBZIP_VERSION_name, &const_LIBZIP_VERSION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(const_LIBZIP_VERSION_name);

	zval const_LENGTH_TO_END_value;
	ZVAL_LONG(&const_LENGTH_TO_END_value, ZIP_LENGTH_TO_END);
	zend_string *const_LENGTH_TO_END_name = zend_string_init_interned("LENGTH_TO_END", sizeof("LENGTH_TO_END") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LENGTH_TO_END_name, &const_LENGTH_TO_END_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LENGTH_TO_END_name);
#if defined(ZIP_LENGTH_UNCHECKED)

	zval const_LENGTH_UNCHECKED_value;
	ZVAL_LONG(&const_LENGTH_UNCHECKED_value, ZIP_LENGTH_UNCHECKED);
	zend_string *const_LENGTH_UNCHECKED_name = zend_string_init_interned("LENGTH_UNCHECKED", sizeof("LENGTH_UNCHECKED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LENGTH_UNCHECKED_name, &const_LENGTH_UNCHECKED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LENGTH_UNCHECKED_name);
#endif

	zval property_lastId_default_value;
	ZVAL_UNDEF(&property_lastId_default_value);
	zend_string *property_lastId_name = zend_string_init("lastId", sizeof("lastId") - 1, 1);
	zend_declare_typed_property(class_entry, property_lastId_name, &property_lastId_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_lastId_name);

	zval property_status_default_value;
	ZVAL_UNDEF(&property_status_default_value);
	zend_string *property_status_name = zend_string_init("status", sizeof("status") - 1, 1);
	zend_declare_typed_property(class_entry, property_status_name, &property_status_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_status_name);

	zval property_statusSys_default_value;
	ZVAL_UNDEF(&property_statusSys_default_value);
	zend_string *property_statusSys_name = zend_string_init("statusSys", sizeof("statusSys") - 1, 1);
	zend_declare_typed_property(class_entry, property_statusSys_name, &property_statusSys_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_statusSys_name);

	zval property_numFiles_default_value;
	ZVAL_UNDEF(&property_numFiles_default_value);
	zend_string *property_numFiles_name = zend_string_init("numFiles", sizeof("numFiles") - 1, 1);
	zend_declare_typed_property(class_entry, property_numFiles_name, &property_numFiles_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_numFiles_name);

	zval property_filename_default_value;
	ZVAL_UNDEF(&property_filename_default_value);
	zend_string *property_filename_name = zend_string_init("filename", sizeof("filename") - 1, 1);
	zend_declare_typed_property(class_entry, property_filename_name, &property_filename_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_filename_name);

	zval property_comment_default_value;
	ZVAL_UNDEF(&property_comment_default_value);
	zend_string *property_comment_name = zend_string_init("comment", sizeof("comment") - 1, 1);
	zend_declare_typed_property(class_entry, property_comment_name, &property_comment_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_comment_name);


	zend_add_parameter_attribute(zend_hash_str_find_ptr(&class_entry->function_table, "setpassword", sizeof("setpassword") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
#if defined(HAVE_ENCRYPTION)

	zend_add_parameter_attribute(zend_hash_str_find_ptr(&class_entry->function_table, "setencryptionname", sizeof("setencryptionname") - 1), 2, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
#endif
#if defined(HAVE_ENCRYPTION)

	zend_add_parameter_attribute(zend_hash_str_find_ptr(&class_entry->function_table, "setencryptionindex", sizeof("setencryptionindex") - 1), 2, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
#endif

	return class_entry;
}
