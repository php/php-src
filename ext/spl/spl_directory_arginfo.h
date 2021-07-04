/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: d9110bb238c9edb5c013bd482649ed96e24ff7b6 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFileInfo___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
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
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, class, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_SplFileInfo_getPathInfo arginfo_class_SplFileInfo_getFileInfo

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFileInfo_openFile, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_STRING, 0, "\"r\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, useIncludePath, _IS_BOOL, 0, "false")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, context, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFileInfo_setFileClass, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, class, IS_STRING, 0, "SplFileObject::class")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFileInfo_setInfoClass, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, class, IS_STRING, 0, "SplFileInfo::class")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_SplFileInfo___toString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplFileInfo___debugInfo arginfo_class_SplFileInfo_getPath

#define arginfo_class_SplFileInfo__bad_state_ex arginfo_class_SplFileInfo_getPath

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DirectoryIterator___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, directory, IS_STRING, 0)
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
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DirectoryIterator___toString arginfo_class_SplFileInfo___toString

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_FilesystemIterator___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, directory, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "FilesystemIterator::KEY_AS_PATHNAME | FilesystemIterator::CURRENT_AS_FILEINFO | FilesystemIterator::SKIP_DOTS")
ZEND_END_ARG_INFO()

#define arginfo_class_FilesystemIterator_rewind arginfo_class_SplFileInfo_getPath

#define arginfo_class_FilesystemIterator_key arginfo_class_SplFileInfo_getPath

#define arginfo_class_FilesystemIterator_current arginfo_class_SplFileInfo_getPath

#define arginfo_class_FilesystemIterator_getFlags arginfo_class_SplFileInfo_getPath

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_FilesystemIterator_setFlags, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RecursiveDirectoryIterator___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, directory, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "FilesystemIterator::KEY_AS_PATHNAME | FilesystemIterator::CURRENT_AS_FILEINFO")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RecursiveDirectoryIterator_hasChildren, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, allowLinks, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

#define arginfo_class_RecursiveDirectoryIterator_getChildren arginfo_class_SplFileInfo_getPath

#define arginfo_class_RecursiveDirectoryIterator_getSubPath arginfo_class_SplFileInfo_getPath

#define arginfo_class_RecursiveDirectoryIterator_getSubPathname arginfo_class_SplFileInfo_getPath

#if defined(HAVE_GLOB)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_GlobIterator___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "FilesystemIterator::KEY_AS_PATHNAME | FilesystemIterator::CURRENT_AS_FILEINFO")
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_GLOB)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_GlobIterator_count, 0, 0, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFileObject___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_STRING, 0, "\"r\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, useIncludePath, _IS_BOOL, 0, "false")
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
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, separator, IS_STRING, 0, "\",\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, enclosure, IS_STRING, 0, "\"\\\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, escape, IS_STRING, 0, "\"\\\\\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFileObject_fputcsv, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, fields, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, separator, IS_STRING, 0, "\",\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, enclosure, IS_STRING, 0, "\"\\\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, escape, IS_STRING, 0, "\"\\\\\"")
ZEND_END_ARG_INFO()

#define arginfo_class_SplFileObject_setCsvControl arginfo_class_SplFileObject_fgetcsv

#define arginfo_class_SplFileObject_getCsvControl arginfo_class_SplFileInfo_getPath

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFileObject_flock, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, operation, IS_LONG, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, wouldBlock, "null")
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
	ZEND_ARG_VARIADIC_TYPE_INFO(1, vars, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFileObject_fwrite, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
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
	ZEND_ARG_TYPE_INFO(0, maxLength, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplFileObject_getMaxLineLen arginfo_class_SplFileInfo_getPath

#define arginfo_class_SplFileObject_hasChildren arginfo_class_SplFileInfo_getPath

#define arginfo_class_SplFileObject_getChildren arginfo_class_SplFileInfo_getPath

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFileObject_seek, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, line, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplFileObject_getCurrentLine arginfo_class_SplFileInfo_getPath

#define arginfo_class_SplFileObject___toString arginfo_class_SplFileInfo___toString

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplTempFileObject___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, maxMemory, IS_LONG, 0, "2 * 1024 * 1024")
ZEND_END_ARG_INFO()


ZEND_METHOD(SplFileInfo, __construct);
ZEND_METHOD(SplFileInfo, getPath);
ZEND_METHOD(SplFileInfo, getFilename);
ZEND_METHOD(SplFileInfo, getExtension);
ZEND_METHOD(SplFileInfo, getBasename);
ZEND_METHOD(SplFileInfo, getPathname);
ZEND_METHOD(SplFileInfo, getPerms);
ZEND_METHOD(SplFileInfo, getInode);
ZEND_METHOD(SplFileInfo, getSize);
ZEND_METHOD(SplFileInfo, getOwner);
ZEND_METHOD(SplFileInfo, getGroup);
ZEND_METHOD(SplFileInfo, getATime);
ZEND_METHOD(SplFileInfo, getMTime);
ZEND_METHOD(SplFileInfo, getCTime);
ZEND_METHOD(SplFileInfo, getType);
ZEND_METHOD(SplFileInfo, isWritable);
ZEND_METHOD(SplFileInfo, isReadable);
ZEND_METHOD(SplFileInfo, isExecutable);
ZEND_METHOD(SplFileInfo, isFile);
ZEND_METHOD(SplFileInfo, isDir);
ZEND_METHOD(SplFileInfo, isLink);
ZEND_METHOD(SplFileInfo, getLinkTarget);
ZEND_METHOD(SplFileInfo, getRealPath);
ZEND_METHOD(SplFileInfo, getFileInfo);
ZEND_METHOD(SplFileInfo, getPathInfo);
ZEND_METHOD(SplFileInfo, openFile);
ZEND_METHOD(SplFileInfo, setFileClass);
ZEND_METHOD(SplFileInfo, setInfoClass);
ZEND_METHOD(SplFileInfo, __debugInfo);
ZEND_METHOD(SplFileInfo, _bad_state_ex);
ZEND_METHOD(DirectoryIterator, __construct);
ZEND_METHOD(DirectoryIterator, getFilename);
ZEND_METHOD(DirectoryIterator, getExtension);
ZEND_METHOD(DirectoryIterator, getBasename);
ZEND_METHOD(DirectoryIterator, isDot);
ZEND_METHOD(DirectoryIterator, rewind);
ZEND_METHOD(DirectoryIterator, valid);
ZEND_METHOD(DirectoryIterator, key);
ZEND_METHOD(DirectoryIterator, current);
ZEND_METHOD(DirectoryIterator, next);
ZEND_METHOD(DirectoryIterator, seek);
ZEND_METHOD(FilesystemIterator, __construct);
ZEND_METHOD(FilesystemIterator, rewind);
ZEND_METHOD(FilesystemIterator, key);
ZEND_METHOD(FilesystemIterator, current);
ZEND_METHOD(FilesystemIterator, getFlags);
ZEND_METHOD(FilesystemIterator, setFlags);
ZEND_METHOD(RecursiveDirectoryIterator, __construct);
ZEND_METHOD(RecursiveDirectoryIterator, hasChildren);
ZEND_METHOD(RecursiveDirectoryIterator, getChildren);
ZEND_METHOD(RecursiveDirectoryIterator, getSubPath);
ZEND_METHOD(RecursiveDirectoryIterator, getSubPathname);
#if defined(HAVE_GLOB)
ZEND_METHOD(GlobIterator, __construct);
#endif
#if defined(HAVE_GLOB)
ZEND_METHOD(GlobIterator, count);
#endif
ZEND_METHOD(SplFileObject, __construct);
ZEND_METHOD(SplFileObject, rewind);
ZEND_METHOD(SplFileObject, eof);
ZEND_METHOD(SplFileObject, valid);
ZEND_METHOD(SplFileObject, fgets);
ZEND_METHOD(SplFileObject, fread);
ZEND_METHOD(SplFileObject, fgetcsv);
ZEND_METHOD(SplFileObject, fputcsv);
ZEND_METHOD(SplFileObject, setCsvControl);
ZEND_METHOD(SplFileObject, getCsvControl);
ZEND_METHOD(SplFileObject, flock);
ZEND_METHOD(SplFileObject, fflush);
ZEND_METHOD(SplFileObject, ftell);
ZEND_METHOD(SplFileObject, fseek);
ZEND_METHOD(SplFileObject, fgetc);
ZEND_METHOD(SplFileObject, fpassthru);
ZEND_METHOD(SplFileObject, fscanf);
ZEND_METHOD(SplFileObject, fwrite);
ZEND_METHOD(SplFileObject, fstat);
ZEND_METHOD(SplFileObject, ftruncate);
ZEND_METHOD(SplFileObject, current);
ZEND_METHOD(SplFileObject, key);
ZEND_METHOD(SplFileObject, next);
ZEND_METHOD(SplFileObject, setFlags);
ZEND_METHOD(SplFileObject, getFlags);
ZEND_METHOD(SplFileObject, setMaxLineLen);
ZEND_METHOD(SplFileObject, getMaxLineLen);
ZEND_METHOD(SplFileObject, hasChildren);
ZEND_METHOD(SplFileObject, getChildren);
ZEND_METHOD(SplFileObject, seek);
ZEND_METHOD(SplTempFileObject, __construct);


static const zend_function_entry class_SplFileInfo_methods[] = {
	ZEND_ME(SplFileInfo, __construct, arginfo_class_SplFileInfo___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileInfo, getPath, arginfo_class_SplFileInfo_getPath, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileInfo, getFilename, arginfo_class_SplFileInfo_getFilename, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileInfo, getExtension, arginfo_class_SplFileInfo_getExtension, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileInfo, getBasename, arginfo_class_SplFileInfo_getBasename, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileInfo, getPathname, arginfo_class_SplFileInfo_getPathname, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileInfo, getPerms, arginfo_class_SplFileInfo_getPerms, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileInfo, getInode, arginfo_class_SplFileInfo_getInode, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileInfo, getSize, arginfo_class_SplFileInfo_getSize, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileInfo, getOwner, arginfo_class_SplFileInfo_getOwner, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileInfo, getGroup, arginfo_class_SplFileInfo_getGroup, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileInfo, getATime, arginfo_class_SplFileInfo_getATime, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileInfo, getMTime, arginfo_class_SplFileInfo_getMTime, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileInfo, getCTime, arginfo_class_SplFileInfo_getCTime, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileInfo, getType, arginfo_class_SplFileInfo_getType, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileInfo, isWritable, arginfo_class_SplFileInfo_isWritable, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileInfo, isReadable, arginfo_class_SplFileInfo_isReadable, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileInfo, isExecutable, arginfo_class_SplFileInfo_isExecutable, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileInfo, isFile, arginfo_class_SplFileInfo_isFile, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileInfo, isDir, arginfo_class_SplFileInfo_isDir, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileInfo, isLink, arginfo_class_SplFileInfo_isLink, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileInfo, getLinkTarget, arginfo_class_SplFileInfo_getLinkTarget, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileInfo, getRealPath, arginfo_class_SplFileInfo_getRealPath, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileInfo, getFileInfo, arginfo_class_SplFileInfo_getFileInfo, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileInfo, getPathInfo, arginfo_class_SplFileInfo_getPathInfo, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileInfo, openFile, arginfo_class_SplFileInfo_openFile, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileInfo, setFileClass, arginfo_class_SplFileInfo_setFileClass, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileInfo, setInfoClass, arginfo_class_SplFileInfo_setInfoClass, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(SplFileInfo, __toString, getPathname, arginfo_class_SplFileInfo___toString, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileInfo, __debugInfo, arginfo_class_SplFileInfo___debugInfo, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileInfo, _bad_state_ex, arginfo_class_SplFileInfo__bad_state_ex, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	ZEND_FE_END
};


static const zend_function_entry class_DirectoryIterator_methods[] = {
	ZEND_ME(DirectoryIterator, __construct, arginfo_class_DirectoryIterator___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(DirectoryIterator, getFilename, arginfo_class_DirectoryIterator_getFilename, ZEND_ACC_PUBLIC)
	ZEND_ME(DirectoryIterator, getExtension, arginfo_class_DirectoryIterator_getExtension, ZEND_ACC_PUBLIC)
	ZEND_ME(DirectoryIterator, getBasename, arginfo_class_DirectoryIterator_getBasename, ZEND_ACC_PUBLIC)
	ZEND_ME(DirectoryIterator, isDot, arginfo_class_DirectoryIterator_isDot, ZEND_ACC_PUBLIC)
	ZEND_ME(DirectoryIterator, rewind, arginfo_class_DirectoryIterator_rewind, ZEND_ACC_PUBLIC)
	ZEND_ME(DirectoryIterator, valid, arginfo_class_DirectoryIterator_valid, ZEND_ACC_PUBLIC)
	ZEND_ME(DirectoryIterator, key, arginfo_class_DirectoryIterator_key, ZEND_ACC_PUBLIC)
	ZEND_ME(DirectoryIterator, current, arginfo_class_DirectoryIterator_current, ZEND_ACC_PUBLIC)
	ZEND_ME(DirectoryIterator, next, arginfo_class_DirectoryIterator_next, ZEND_ACC_PUBLIC)
	ZEND_ME(DirectoryIterator, seek, arginfo_class_DirectoryIterator_seek, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(DirectoryIterator, __toString, getFilename, arginfo_class_DirectoryIterator___toString, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_FilesystemIterator_methods[] = {
	ZEND_ME(FilesystemIterator, __construct, arginfo_class_FilesystemIterator___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(FilesystemIterator, rewind, arginfo_class_FilesystemIterator_rewind, ZEND_ACC_PUBLIC)
	ZEND_ME(FilesystemIterator, key, arginfo_class_FilesystemIterator_key, ZEND_ACC_PUBLIC)
	ZEND_ME(FilesystemIterator, current, arginfo_class_FilesystemIterator_current, ZEND_ACC_PUBLIC)
	ZEND_ME(FilesystemIterator, getFlags, arginfo_class_FilesystemIterator_getFlags, ZEND_ACC_PUBLIC)
	ZEND_ME(FilesystemIterator, setFlags, arginfo_class_FilesystemIterator_setFlags, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_RecursiveDirectoryIterator_methods[] = {
	ZEND_ME(RecursiveDirectoryIterator, __construct, arginfo_class_RecursiveDirectoryIterator___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveDirectoryIterator, hasChildren, arginfo_class_RecursiveDirectoryIterator_hasChildren, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveDirectoryIterator, getChildren, arginfo_class_RecursiveDirectoryIterator_getChildren, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveDirectoryIterator, getSubPath, arginfo_class_RecursiveDirectoryIterator_getSubPath, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveDirectoryIterator, getSubPathname, arginfo_class_RecursiveDirectoryIterator_getSubPathname, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_GlobIterator_methods[] = {
#if defined(HAVE_GLOB)
	ZEND_ME(GlobIterator, __construct, arginfo_class_GlobIterator___construct, ZEND_ACC_PUBLIC)
#endif
#if defined(HAVE_GLOB)
	ZEND_ME(GlobIterator, count, arginfo_class_GlobIterator_count, ZEND_ACC_PUBLIC)
#endif
	ZEND_FE_END
};


static const zend_function_entry class_SplFileObject_methods[] = {
	ZEND_ME(SplFileObject, __construct, arginfo_class_SplFileObject___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileObject, rewind, arginfo_class_SplFileObject_rewind, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileObject, eof, arginfo_class_SplFileObject_eof, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileObject, valid, arginfo_class_SplFileObject_valid, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileObject, fgets, arginfo_class_SplFileObject_fgets, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileObject, fread, arginfo_class_SplFileObject_fread, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileObject, fgetcsv, arginfo_class_SplFileObject_fgetcsv, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileObject, fputcsv, arginfo_class_SplFileObject_fputcsv, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileObject, setCsvControl, arginfo_class_SplFileObject_setCsvControl, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileObject, getCsvControl, arginfo_class_SplFileObject_getCsvControl, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileObject, flock, arginfo_class_SplFileObject_flock, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileObject, fflush, arginfo_class_SplFileObject_fflush, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileObject, ftell, arginfo_class_SplFileObject_ftell, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileObject, fseek, arginfo_class_SplFileObject_fseek, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileObject, fgetc, arginfo_class_SplFileObject_fgetc, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileObject, fpassthru, arginfo_class_SplFileObject_fpassthru, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileObject, fscanf, arginfo_class_SplFileObject_fscanf, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileObject, fwrite, arginfo_class_SplFileObject_fwrite, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileObject, fstat, arginfo_class_SplFileObject_fstat, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileObject, ftruncate, arginfo_class_SplFileObject_ftruncate, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileObject, current, arginfo_class_SplFileObject_current, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileObject, key, arginfo_class_SplFileObject_key, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileObject, next, arginfo_class_SplFileObject_next, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileObject, setFlags, arginfo_class_SplFileObject_setFlags, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileObject, getFlags, arginfo_class_SplFileObject_getFlags, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileObject, setMaxLineLen, arginfo_class_SplFileObject_setMaxLineLen, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileObject, getMaxLineLen, arginfo_class_SplFileObject_getMaxLineLen, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileObject, hasChildren, arginfo_class_SplFileObject_hasChildren, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileObject, getChildren, arginfo_class_SplFileObject_getChildren, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFileObject, seek, arginfo_class_SplFileObject_seek, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(SplFileObject, getCurrentLine, fgets, arginfo_class_SplFileObject_getCurrentLine, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(SplFileObject, __toString, fgets, arginfo_class_SplFileObject___toString, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_SplTempFileObject_methods[] = {
	ZEND_ME(SplTempFileObject, __construct, arginfo_class_SplTempFileObject___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
