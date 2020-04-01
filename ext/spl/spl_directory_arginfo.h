/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFileInfo___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, file_name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFileInfo_getPath, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplFileInfo_getFilename arginfo_class_SplFileInfo_getPath

#define arginfo_class_SplFileInfo_getExtension arginfo_class_SplFileInfo_getPath

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFileInfo_getBasename, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, suffix, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

#define arginfo_class_SplFileInfo_getPathname arginfo_class_SplFileInfo_getPath

#define arginfo_class_SplFileInfo_getPerms arginfo_class_SplFileInfo_getPath

#define arginfo_class_SplFileInfo_getInode arginfo_class_SplFileInfo_getPath

#define arginfo_class_SplFileInfo_getSize arginfo_class_SplFileInfo_getPath

#define arginfo_class_SplFileInfo_getOwner arginfo_class_SplFileInfo_getPath

#define arginfo_class_SplFileInfo_getGroup arginfo_class_SplFileInfo_getPath

#define arginfo_class_SplFileInfo_getATime arginfo_class_SplFileInfo_getPath

#define arginfo_class_SplFileInfo_getMTime arginfo_class_SplFileInfo_getPath

#define arginfo_class_SplFileInfo_getCTime arginfo_class_SplFileInfo_getPath

#define arginfo_class_SplFileInfo_getType arginfo_class_SplFileInfo_getPath

#define arginfo_class_SplFileInfo_isWritable arginfo_class_SplFileInfo_getPath

#define arginfo_class_SplFileInfo_isReadable arginfo_class_SplFileInfo_getPath

#define arginfo_class_SplFileInfo_isExecutable arginfo_class_SplFileInfo_getPath

#define arginfo_class_SplFileInfo_isFile arginfo_class_SplFileInfo_getPath

#define arginfo_class_SplFileInfo_isDir arginfo_class_SplFileInfo_getPath

#define arginfo_class_SplFileInfo_isLink arginfo_class_SplFileInfo_getPath

#define arginfo_class_SplFileInfo_getLinkTarget arginfo_class_SplFileInfo_getPath

#define arginfo_class_SplFileInfo_getRealPath arginfo_class_SplFileInfo_getPath

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFileInfo_getFileInfo, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, class_name, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplFileInfo_getPathInfo arginfo_class_SplFileInfo_getFileInfo

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFileInfo_openFile, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, open_mode, IS_STRING, 0, "\'r\'")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, use_include_path, _IS_BOOL, 0, "false")
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFileInfo_setFileClass, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, class_name, IS_STRING, 0, "SplFileObject::class")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFileInfo_setInfoClass, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, class_name, IS_STRING, 0, "SplFileInfo::class")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_SplFileInfo___toString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplFileInfo__bad_state_ex arginfo_class_SplFileInfo_getPath

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DirectoryIterator___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DirectoryIterator_getFilename arginfo_class_SplFileInfo_getPath

#define arginfo_class_DirectoryIterator_getExtension arginfo_class_SplFileInfo_getPath

#define arginfo_class_DirectoryIterator_getBasename arginfo_class_SplFileInfo_getBasename

#define arginfo_class_DirectoryIterator_isDot arginfo_class_SplFileInfo_getPath

#define arginfo_class_DirectoryIterator_rewind arginfo_class_SplFileInfo_getPath

#define arginfo_class_DirectoryIterator_valid arginfo_class_SplFileInfo_getPath

#define arginfo_class_DirectoryIterator_key arginfo_class_SplFileInfo_getPath

#define arginfo_class_DirectoryIterator_current arginfo_class_SplFileInfo_getPath

#define arginfo_class_DirectoryIterator_next arginfo_class_SplFileInfo_getPath

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DirectoryIterator_seek, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, position, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DirectoryIterator___toString arginfo_class_SplFileInfo___toString

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_FilesystemIterator___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "FilesystemIterator::KEY_AS_PATHNAME | FilesystemIterator::CURRENT_AS_FILEINFO | FilesystemIterator::SKIP_DOTS")
ZEND_END_ARG_INFO()

#define arginfo_class_FilesystemIterator_rewind arginfo_class_SplFileInfo_getPath

#define arginfo_class_FilesystemIterator_next arginfo_class_SplFileInfo_getPath

#define arginfo_class_FilesystemIterator_key arginfo_class_SplFileInfo_getPath

#define arginfo_class_FilesystemIterator_current arginfo_class_SplFileInfo_getPath

#define arginfo_class_FilesystemIterator_getFlags arginfo_class_SplFileInfo_getPath

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_FilesystemIterator_setFlags, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RecursiveDirectoryIterator___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "FilesystemIterator::KEY_AS_PATHNAME | FilesystemIterator::CURRENT_AS_FILEINFO")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RecursiveDirectoryIterator_hasChildren, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, allow_links, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

#define arginfo_class_RecursiveDirectoryIterator_getChildren arginfo_class_SplFileInfo_getPath

#define arginfo_class_RecursiveDirectoryIterator_getSubPath arginfo_class_SplFileInfo_getPath

#define arginfo_class_RecursiveDirectoryIterator_getSubPathname arginfo_class_SplFileInfo_getPath

#define arginfo_class_GlobIterator___construct arginfo_class_RecursiveDirectoryIterator___construct

#define arginfo_class_GlobIterator_count arginfo_class_SplFileInfo_getPath

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFileObject___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, file_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, open_mode, IS_STRING, 0, "\'r\'")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, use_include_path, _IS_BOOL, 0, "false")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, context, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_SplFileObject_rewind arginfo_class_SplFileInfo_getPath

#define arginfo_class_SplFileObject_eof arginfo_class_SplFileInfo_getPath

#define arginfo_class_SplFileObject_valid arginfo_class_SplFileInfo_getPath

#define arginfo_class_SplFileObject_fgets arginfo_class_SplFileInfo_getPath

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFileObject_fread, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFileObject_fgetcsv, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, delimiter, IS_STRING, 0, "\",\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, enclosure, IS_STRING, 0, "\'\"\'")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, escape, "\"\\\\\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFileObject_fputcsv, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, fields, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, delimiter, IS_STRING, 0, "\',\'")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, enclosure, IS_STRING, 0, "\'\"\'")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, escape, IS_STRING, 0, "\"\\\\\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFileObject_setCsvControl, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, delimiter, IS_STRING, 0, "\",\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, enclosure, IS_STRING, 0, "\"\\\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, escape, IS_STRING, 0, "\"\\\\\"")
ZEND_END_ARG_INFO()

#define arginfo_class_SplFileObject_getCsvControl arginfo_class_SplFileInfo_getPath

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFileObject_flock, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, operation, IS_LONG, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, wouldblock, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_SplFileObject_fflush arginfo_class_SplFileInfo_getPath

#define arginfo_class_SplFileObject_ftell arginfo_class_SplFileInfo_getPath

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFileObject_fseek, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, whence, IS_LONG, 0, "SEEK_SET")
ZEND_END_ARG_INFO()

#define arginfo_class_SplFileObject_fgetc arginfo_class_SplFileInfo_getPath

#define arginfo_class_SplFileObject_fpassthru arginfo_class_SplFileInfo_getPath

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFileObject_fscanf, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
	ZEND_ARG_VARIADIC_INFO(1, params)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFileObject_fwrite, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_SplFileObject_fstat arginfo_class_SplFileInfo_getPath

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFileObject_ftruncate, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplFileObject_current arginfo_class_SplFileInfo_getPath

#define arginfo_class_SplFileObject_key arginfo_class_SplFileInfo_getPath

#define arginfo_class_SplFileObject_next arginfo_class_SplFileInfo_getPath

#define arginfo_class_SplFileObject_setFlags arginfo_class_FilesystemIterator_setFlags

#define arginfo_class_SplFileObject_getFlags arginfo_class_SplFileInfo_getPath

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFileObject_setMaxLineLen, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, max_len, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplFileObject_getMaxLineLen arginfo_class_SplFileInfo_getPath

#define arginfo_class_SplFileObject_hasChildren arginfo_class_SplFileInfo_getPath

#define arginfo_class_SplFileObject_getChildren arginfo_class_SplFileInfo_getPath

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFileObject_seek, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, line_pos, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplFileObject_getCurrentLine arginfo_class_SplFileInfo_getPath

#define arginfo_class_SplFileObject___toString arginfo_class_SplFileInfo___toString

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplTempFileObject___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, max_memory, IS_LONG, 0, "2 * 1024 * 1024")
ZEND_END_ARG_INFO()
