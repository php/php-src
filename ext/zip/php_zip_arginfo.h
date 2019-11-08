/* This is a generated file, edit the .stub.php file instead. */

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
	ZEND_ARG_TYPE_INFO(0, mode, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zip_entry_close, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, zip_ent)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_zip_entry_read, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, zip_entry)
	ZEND_ARG_TYPE_INFO(0, len, IS_LONG, 0)
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
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
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
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_addFromString, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, content, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_addFile, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filepath, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, entryname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_addGlob, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_addPattern, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
	ZEND_ARG_INFO(0, options)
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
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_setCommentIndex, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, comment, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_setCommentName, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, comment, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_getCommentIndex, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_getCommentName, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_deleteIndex, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_deleteName, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ZipArchive_statName arginfo_class_ZipArchive_open

#define arginfo_class_ZipArchive_statIndex arginfo_class_ZipArchive_getCommentIndex

#define arginfo_class_ZipArchive_locateName arginfo_class_ZipArchive_open

#define arginfo_class_ZipArchive_getNameIndex arginfo_class_ZipArchive_getCommentIndex

#define arginfo_class_ZipArchive_unchangeArchive arginfo_class_ZipArchive_close

#define arginfo_class_ZipArchive_unchangeAll arginfo_class_ZipArchive_close

#define arginfo_class_ZipArchive_unchangeIndex arginfo_class_ZipArchive_deleteIndex

#define arginfo_class_ZipArchive_unchangeName arginfo_class_ZipArchive_deleteName

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_extractTo, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, pathto, IS_STRING, 0)
	ZEND_ARG_INFO(0, files)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_getFromName, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, entryname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, len, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_getFromIndex, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, len, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_getStream, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, entryname, IS_STRING, 0)
ZEND_END_ARG_INFO()

#if defined(ZIP_OPSYS_DEFAULT)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_setExternalAttributesName, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, opsys, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, attr, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(ZIP_OPSYS_DEFAULT)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_setExternalAttributesIndex, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, opsys, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, attr, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(ZIP_OPSYS_DEFAULT)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_getExternalAttributesName, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_INFO(1, opsys)
	ZEND_ARG_INFO(1, attr)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(ZIP_OPSYS_DEFAULT)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_getExternalAttributesIndex, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_INFO(1, opsys)
	ZEND_ARG_INFO(1, attr)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_setCompressionName, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, method, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, compflags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_setCompressionIndex, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, method, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, compflags, IS_LONG, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_ENCRYPTION)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_setEncryptionName, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, method, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_ENCRYPTION)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZipArchive_setEncryptionIndex, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, method, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif
