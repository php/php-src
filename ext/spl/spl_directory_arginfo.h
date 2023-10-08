/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 6212994df4935f42c99080b928b8b5b08bdbe19d */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFileInfo___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplFileInfo_getPath, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplFileInfo_getFilename arginfo_class_SplFileInfo_getPath

#define arginfo_class_SplFileInfo_getExtension arginfo_class_SplFileInfo_getPath

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplFileInfo_getBasename, 0, 0, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, suffix, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

#define arginfo_class_SplFileInfo_getPathname arginfo_class_SplFileInfo_getPath

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_SplFileInfo_getPerms, 0, 0, MAY_BE_LONG|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

#define arginfo_class_SplFileInfo_getInode arginfo_class_SplFileInfo_getPerms

#define arginfo_class_SplFileInfo_getSize arginfo_class_SplFileInfo_getPerms

#define arginfo_class_SplFileInfo_getOwner arginfo_class_SplFileInfo_getPerms

#define arginfo_class_SplFileInfo_getGroup arginfo_class_SplFileInfo_getPerms

#define arginfo_class_SplFileInfo_getATime arginfo_class_SplFileInfo_getPerms

#define arginfo_class_SplFileInfo_getMTime arginfo_class_SplFileInfo_getPerms

#define arginfo_class_SplFileInfo_getCTime arginfo_class_SplFileInfo_getPerms

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_SplFileInfo_getType, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplFileInfo_isWritable, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplFileInfo_isReadable arginfo_class_SplFileInfo_isWritable

#define arginfo_class_SplFileInfo_isExecutable arginfo_class_SplFileInfo_isWritable

#define arginfo_class_SplFileInfo_isFile arginfo_class_SplFileInfo_isWritable

#define arginfo_class_SplFileInfo_isDir arginfo_class_SplFileInfo_isWritable

#define arginfo_class_SplFileInfo_isLink arginfo_class_SplFileInfo_isWritable

#define arginfo_class_SplFileInfo_getLinkTarget arginfo_class_SplFileInfo_getType

#define arginfo_class_SplFileInfo_getRealPath arginfo_class_SplFileInfo_getType

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_SplFileInfo_getFileInfo, 0, 0, SplFileInfo, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, class, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_SplFileInfo_getPathInfo, 0, 0, SplFileInfo, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, class, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_SplFileInfo_openFile, 0, 0, SplFileObject, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_STRING, 0, "\"r\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, useIncludePath, _IS_BOOL, 0, "false")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, context, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplFileInfo_setFileClass, 0, 0, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, class, IS_STRING, 0, "SplFileObject::class")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplFileInfo_setInfoClass, 0, 0, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, class, IS_STRING, 0, "SplFileInfo::class")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_SplFileInfo___toString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplFileInfo___debugInfo, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplFileInfo__bad_state_ex, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DirectoryIterator___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, directory, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DirectoryIterator_getFilename arginfo_class_SplFileInfo_getPath

#define arginfo_class_DirectoryIterator_getExtension arginfo_class_SplFileInfo_getPath

#define arginfo_class_DirectoryIterator_getBasename arginfo_class_SplFileInfo_getBasename

#define arginfo_class_DirectoryIterator_isDot arginfo_class_SplFileInfo_isWritable

#define arginfo_class_DirectoryIterator_rewind arginfo_class_SplFileInfo__bad_state_ex

#define arginfo_class_DirectoryIterator_valid arginfo_class_SplFileInfo_isWritable

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DirectoryIterator_key, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DirectoryIterator_current arginfo_class_DirectoryIterator_key

#define arginfo_class_DirectoryIterator_next arginfo_class_SplFileInfo__bad_state_ex

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DirectoryIterator_seek, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DirectoryIterator___toString arginfo_class_SplFileInfo___toString

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_FilesystemIterator___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, directory, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "FilesystemIterator::KEY_AS_PATHNAME | FilesystemIterator::CURRENT_AS_FILEINFO | FilesystemIterator::SKIP_DOTS")
ZEND_END_ARG_INFO()

#define arginfo_class_FilesystemIterator_rewind arginfo_class_SplFileInfo__bad_state_ex

#define arginfo_class_FilesystemIterator_key arginfo_class_SplFileInfo_getPath

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_FilesystemIterator_current, 0, 0, SplFileInfo|FilesystemIterator, MAY_BE_STRING)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_FilesystemIterator_getFlags, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_FilesystemIterator_setFlags, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RecursiveDirectoryIterator___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, directory, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "FilesystemIterator::KEY_AS_PATHNAME | FilesystemIterator::CURRENT_AS_FILEINFO")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_RecursiveDirectoryIterator_hasChildren, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, allowLinks, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_RecursiveDirectoryIterator_getChildren, 0, 0, RecursiveDirectoryIterator, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_RecursiveDirectoryIterator_getSubPath arginfo_class_SplFileInfo_getPath

#define arginfo_class_RecursiveDirectoryIterator_getSubPathname arginfo_class_SplFileInfo_getPath

#if defined(HAVE_GLOB)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_GlobIterator___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "FilesystemIterator::KEY_AS_PATHNAME | FilesystemIterator::CURRENT_AS_FILEINFO")
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_GLOB)
ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_GlobIterator_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFileObject___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_STRING, 0, "\"r\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, useIncludePath, _IS_BOOL, 0, "false")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, context, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_SplFileObject_rewind arginfo_class_SplFileInfo__bad_state_ex

#define arginfo_class_SplFileObject_eof arginfo_class_SplFileInfo_isWritable

#define arginfo_class_SplFileObject_valid arginfo_class_SplFileInfo_isWritable

#define arginfo_class_SplFileObject_fgets arginfo_class_SplFileInfo_getPath

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_SplFileObject_fread, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_SplFileObject_fgetcsv, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, separator, IS_STRING, 0, "\",\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, enclosure, IS_STRING, 0, "\"\\\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, escape, IS_STRING, 0, "\"\\\\\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_SplFileObject_fputcsv, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, fields, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, separator, IS_STRING, 0, "\",\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, enclosure, IS_STRING, 0, "\"\\\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, escape, IS_STRING, 0, "\"\\\\\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, eol, IS_STRING, 0, "\"\\n\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplFileObject_setCsvControl, 0, 0, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, separator, IS_STRING, 0, "\",\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, enclosure, IS_STRING, 0, "\"\\\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, escape, IS_STRING, 0, "\"\\\\\"")
ZEND_END_ARG_INFO()

#define arginfo_class_SplFileObject_getCsvControl arginfo_class_SplFileInfo___debugInfo

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplFileObject_flock, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, operation, IS_LONG, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, wouldBlock, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_SplFileObject_fflush arginfo_class_SplFileInfo_isWritable

#define arginfo_class_SplFileObject_ftell arginfo_class_SplFileInfo_getPerms

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplFileObject_fseek, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, whence, IS_LONG, 0, "SEEK_SET")
ZEND_END_ARG_INFO()

#define arginfo_class_SplFileObject_fgetc arginfo_class_SplFileInfo_getType

#define arginfo_class_SplFileObject_fpassthru arginfo_class_FilesystemIterator_getFlags

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_SplFileObject_fscanf, 0, 1, MAY_BE_ARRAY|MAY_BE_LONG|MAY_BE_NULL)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(1, vars, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_SplFileObject_fwrite, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_SplFileObject_fstat arginfo_class_SplFileInfo___debugInfo

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplFileObject_ftruncate, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_SplFileObject_current, 0, 0, MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

#define arginfo_class_SplFileObject_key arginfo_class_FilesystemIterator_getFlags

#define arginfo_class_SplFileObject_next arginfo_class_SplFileInfo__bad_state_ex

#define arginfo_class_SplFileObject_setFlags arginfo_class_FilesystemIterator_setFlags

#define arginfo_class_SplFileObject_getFlags arginfo_class_FilesystemIterator_getFlags

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplFileObject_setMaxLineLen, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, maxLength, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplFileObject_getMaxLineLen arginfo_class_FilesystemIterator_getFlags

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplFileObject_hasChildren, 0, 0, IS_FALSE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplFileObject_getChildren, 0, 0, IS_NULL, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplFileObject_seek, 0, 1, IS_VOID, 0)
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
ZEND_METHOD(SplFileObject, __toString);
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
	ZEND_ME(SplFileInfo, _bad_state_ex, arginfo_class_SplFileInfo__bad_state_ex, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL|ZEND_ACC_DEPRECATED)
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


#if defined(HAVE_GLOB)
static const zend_function_entry class_GlobIterator_methods[] = {
	ZEND_ME(GlobIterator, __construct, arginfo_class_GlobIterator___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(GlobIterator, count, arginfo_class_GlobIterator_count, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
#endif


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
	ZEND_ME(SplFileObject, __toString, arginfo_class_SplFileObject___toString, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_SplTempFileObject_methods[] = {
	ZEND_ME(SplTempFileObject, __construct, arginfo_class_SplTempFileObject___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_SplFileInfo(zend_class_entry *class_entry_Stringable)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SplFileInfo", class_SplFileInfo_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;
	zend_class_implements(class_entry, 1, class_entry_Stringable);

	return class_entry;
}

static zend_class_entry *register_class_DirectoryIterator(zend_class_entry *class_entry_SplFileInfo, zend_class_entry *class_entry_SeekableIterator)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DirectoryIterator", class_DirectoryIterator_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_SplFileInfo);
	zend_class_implements(class_entry, 1, class_entry_SeekableIterator);

	return class_entry;
}

static zend_class_entry *register_class_FilesystemIterator(zend_class_entry *class_entry_DirectoryIterator)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "FilesystemIterator", class_FilesystemIterator_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_DirectoryIterator);

	zval const_CURRENT_MODE_MASK_value;
	ZVAL_LONG(&const_CURRENT_MODE_MASK_value, SPL_FILE_DIR_CURRENT_MODE_MASK);
	zend_string *const_CURRENT_MODE_MASK_name = zend_string_init_interned("CURRENT_MODE_MASK", sizeof("CURRENT_MODE_MASK") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CURRENT_MODE_MASK_name, &const_CURRENT_MODE_MASK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CURRENT_MODE_MASK_name);

	zval const_CURRENT_AS_PATHNAME_value;
	ZVAL_LONG(&const_CURRENT_AS_PATHNAME_value, SPL_FILE_DIR_CURRENT_AS_PATHNAME);
	zend_string *const_CURRENT_AS_PATHNAME_name = zend_string_init_interned("CURRENT_AS_PATHNAME", sizeof("CURRENT_AS_PATHNAME") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CURRENT_AS_PATHNAME_name, &const_CURRENT_AS_PATHNAME_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CURRENT_AS_PATHNAME_name);

	zval const_CURRENT_AS_FILEINFO_value;
	ZVAL_LONG(&const_CURRENT_AS_FILEINFO_value, SPL_FILE_DIR_CURRENT_AS_FILEINFO);
	zend_string *const_CURRENT_AS_FILEINFO_name = zend_string_init_interned("CURRENT_AS_FILEINFO", sizeof("CURRENT_AS_FILEINFO") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CURRENT_AS_FILEINFO_name, &const_CURRENT_AS_FILEINFO_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CURRENT_AS_FILEINFO_name);

	zval const_CURRENT_AS_SELF_value;
	ZVAL_LONG(&const_CURRENT_AS_SELF_value, SPL_FILE_DIR_CURRENT_AS_SELF);
	zend_string *const_CURRENT_AS_SELF_name = zend_string_init_interned("CURRENT_AS_SELF", sizeof("CURRENT_AS_SELF") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CURRENT_AS_SELF_name, &const_CURRENT_AS_SELF_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CURRENT_AS_SELF_name);

	zval const_KEY_MODE_MASK_value;
	ZVAL_LONG(&const_KEY_MODE_MASK_value, SPL_FILE_DIR_KEY_MODE_MASK);
	zend_string *const_KEY_MODE_MASK_name = zend_string_init_interned("KEY_MODE_MASK", sizeof("KEY_MODE_MASK") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_KEY_MODE_MASK_name, &const_KEY_MODE_MASK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_KEY_MODE_MASK_name);

	zval const_KEY_AS_PATHNAME_value;
	ZVAL_LONG(&const_KEY_AS_PATHNAME_value, SPL_FILE_DIR_KEY_AS_PATHNAME);
	zend_string *const_KEY_AS_PATHNAME_name = zend_string_init_interned("KEY_AS_PATHNAME", sizeof("KEY_AS_PATHNAME") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_KEY_AS_PATHNAME_name, &const_KEY_AS_PATHNAME_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_KEY_AS_PATHNAME_name);

	zval const_FOLLOW_SYMLINKS_value;
	ZVAL_LONG(&const_FOLLOW_SYMLINKS_value, SPL_FILE_DIR_FOLLOW_SYMLINKS);
	zend_string *const_FOLLOW_SYMLINKS_name = zend_string_init_interned("FOLLOW_SYMLINKS", sizeof("FOLLOW_SYMLINKS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FOLLOW_SYMLINKS_name, &const_FOLLOW_SYMLINKS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FOLLOW_SYMLINKS_name);

	zval const_KEY_AS_FILENAME_value;
	ZVAL_LONG(&const_KEY_AS_FILENAME_value, SPL_FILE_DIR_KEY_AS_FILENAME);
	zend_string *const_KEY_AS_FILENAME_name = zend_string_init_interned("KEY_AS_FILENAME", sizeof("KEY_AS_FILENAME") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_KEY_AS_FILENAME_name, &const_KEY_AS_FILENAME_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_KEY_AS_FILENAME_name);

	zval const_NEW_CURRENT_AND_KEY_value;
	ZVAL_LONG(&const_NEW_CURRENT_AND_KEY_value, SPL_FILE_NEW_CURRENT_AND_KEY);
	zend_string *const_NEW_CURRENT_AND_KEY_name = zend_string_init_interned("NEW_CURRENT_AND_KEY", sizeof("NEW_CURRENT_AND_KEY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_NEW_CURRENT_AND_KEY_name, &const_NEW_CURRENT_AND_KEY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_NEW_CURRENT_AND_KEY_name);

	zval const_OTHER_MODE_MASK_value;
	ZVAL_LONG(&const_OTHER_MODE_MASK_value, SPL_FILE_DIR_OTHERS_MASK);
	zend_string *const_OTHER_MODE_MASK_name = zend_string_init_interned("OTHER_MODE_MASK", sizeof("OTHER_MODE_MASK") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_OTHER_MODE_MASK_name, &const_OTHER_MODE_MASK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_OTHER_MODE_MASK_name);

	zval const_SKIP_DOTS_value;
	ZVAL_LONG(&const_SKIP_DOTS_value, SPL_FILE_DIR_SKIPDOTS);
	zend_string *const_SKIP_DOTS_name = zend_string_init_interned("SKIP_DOTS", sizeof("SKIP_DOTS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_SKIP_DOTS_name, &const_SKIP_DOTS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_SKIP_DOTS_name);

	zval const_UNIX_PATHS_value;
	ZVAL_LONG(&const_UNIX_PATHS_value, SPL_FILE_DIR_UNIXPATHS);
	zend_string *const_UNIX_PATHS_name = zend_string_init_interned("UNIX_PATHS", sizeof("UNIX_PATHS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_UNIX_PATHS_name, &const_UNIX_PATHS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_UNIX_PATHS_name);

	return class_entry;
}

static zend_class_entry *register_class_RecursiveDirectoryIterator(zend_class_entry *class_entry_FilesystemIterator, zend_class_entry *class_entry_RecursiveIterator)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "RecursiveDirectoryIterator", class_RecursiveDirectoryIterator_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_FilesystemIterator);
	zend_class_implements(class_entry, 1, class_entry_RecursiveIterator);

	return class_entry;
}

#if defined(HAVE_GLOB)
static zend_class_entry *register_class_GlobIterator(zend_class_entry *class_entry_FilesystemIterator, zend_class_entry *class_entry_Countable)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "GlobIterator", class_GlobIterator_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_FilesystemIterator);
	zend_class_implements(class_entry, 1, class_entry_Countable);

	return class_entry;
}
#endif

static zend_class_entry *register_class_SplFileObject(zend_class_entry *class_entry_SplFileInfo, zend_class_entry *class_entry_RecursiveIterator, zend_class_entry *class_entry_SeekableIterator)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SplFileObject", class_SplFileObject_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_SplFileInfo);
	zend_class_implements(class_entry, 2, class_entry_RecursiveIterator, class_entry_SeekableIterator);

	zval const_DROP_NEW_LINE_value;
	ZVAL_LONG(&const_DROP_NEW_LINE_value, SPL_FILE_OBJECT_DROP_NEW_LINE);
	zend_string *const_DROP_NEW_LINE_name = zend_string_init_interned("DROP_NEW_LINE", sizeof("DROP_NEW_LINE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DROP_NEW_LINE_name, &const_DROP_NEW_LINE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DROP_NEW_LINE_name);

	zval const_READ_AHEAD_value;
	ZVAL_LONG(&const_READ_AHEAD_value, SPL_FILE_OBJECT_READ_AHEAD);
	zend_string *const_READ_AHEAD_name = zend_string_init_interned("READ_AHEAD", sizeof("READ_AHEAD") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_READ_AHEAD_name, &const_READ_AHEAD_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_READ_AHEAD_name);

	zval const_SKIP_EMPTY_value;
	ZVAL_LONG(&const_SKIP_EMPTY_value, SPL_FILE_OBJECT_SKIP_EMPTY);
	zend_string *const_SKIP_EMPTY_name = zend_string_init_interned("SKIP_EMPTY", sizeof("SKIP_EMPTY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_SKIP_EMPTY_name, &const_SKIP_EMPTY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_SKIP_EMPTY_name);

	zval const_READ_CSV_value;
	ZVAL_LONG(&const_READ_CSV_value, SPL_FILE_OBJECT_READ_CSV);
	zend_string *const_READ_CSV_name = zend_string_init_interned("READ_CSV", sizeof("READ_CSV") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_READ_CSV_name, &const_READ_CSV_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_READ_CSV_name);

	return class_entry;
}

static zend_class_entry *register_class_SplTempFileObject(zend_class_entry *class_entry_SplFileObject)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SplTempFileObject", class_SplTempFileObject_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_SplFileObject);

	return class_entry;
}
