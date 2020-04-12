/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Phar___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "FilesystemIterator::SKIP_DOTS | FilesystemIterator::UNIX_PATHS")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, alias, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Phar___destruct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Phar_addEmptyDir, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, dirname, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Phar_addFile, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, localname, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Phar_addFromString, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, localname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, contents, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Phar_buildFromDirectory, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, base_dir, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, regex, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Phar_buildFromIterator, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, iterator, Traversable, 0)
	ZEND_ARG_TYPE_INFO(0, base_directory, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Phar_compressFiles, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, compression_type, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Phar_decompressFiles arginfo_class_Phar___destruct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Phar_compress, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, compression_type, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, file_ext, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Phar_decompress, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, file_ext, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Phar_convertToExecutable, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, format, IS_LONG, 0, "9021976")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, compression_type, IS_LONG, 0, "9021976")
	ZEND_ARG_TYPE_INFO(0, file_ext, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Phar_convertToData arginfo_class_Phar_convertToExecutable

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Phar_copy, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, newfile, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, oldfile, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Phar_count, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Phar_delete, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, entry, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Phar_delMetadata arginfo_class_Phar___destruct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Phar_extractTo, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, pathto, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, files, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, overwrite, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

#define arginfo_class_Phar_getAlias arginfo_class_Phar___destruct

#define arginfo_class_Phar_getPath arginfo_class_Phar___destruct

#define arginfo_class_Phar_getMetadata arginfo_class_Phar___destruct

#define arginfo_class_Phar_getModified arginfo_class_Phar___destruct

#define arginfo_class_Phar_getSignature arginfo_class_Phar___destruct

#define arginfo_class_Phar_getStub arginfo_class_Phar___destruct

#define arginfo_class_Phar_getVersion arginfo_class_Phar___destruct

#define arginfo_class_Phar_hasMetadata arginfo_class_Phar___destruct

#define arginfo_class_Phar_isBuffering arginfo_class_Phar___destruct

#define arginfo_class_Phar_isCompressed arginfo_class_Phar___destruct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Phar_isFileFormat, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, fileformat, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Phar_isWritable arginfo_class_Phar___destruct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Phar_offsetExists, 0, 0, 1)
	ZEND_ARG_INFO(0, entry)
ZEND_END_ARG_INFO()

#define arginfo_class_Phar_offsetGet arginfo_class_Phar_offsetExists

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Phar_offsetSet, 0, 0, 2)
	ZEND_ARG_INFO(0, entry)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

#define arginfo_class_Phar_offsetUnset arginfo_class_Phar_offsetExists

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Phar_setAlias, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, alias, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Phar_setDefaultStub, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, index, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO(0, webindex, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Phar_setMetadata, 0, 0, 1)
	ZEND_ARG_INFO(0, metadata)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Phar_setSignatureAlgorithm, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, algorithm, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, privatekey, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Phar_setStub, 0, 0, 1)
	ZEND_ARG_INFO(0, newstub)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, maxlen, "-1")
ZEND_END_ARG_INFO()

#define arginfo_class_Phar_startBuffering arginfo_class_Phar___destruct

#define arginfo_class_Phar_stopBuffering arginfo_class_Phar___destruct

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Phar_apiVersion, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Phar_canCompress, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, method, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Phar_canWrite, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Phar_createDefaultStub, 0, 0, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, webindex, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Phar_getSupportedCompression, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Phar_getSupportedSignatures arginfo_class_Phar_getSupportedCompression

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Phar_interceptFileFuncs, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Phar_isValidPharFilename, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, executable, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Phar_loadPhar, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, alias, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Phar_mapPhar, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, alias, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Phar_running, 0, 0, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, retphar, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Phar_mount, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, inphar, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, externalfile, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Phar_mungServer, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, munglist, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Phar_unlinkArchive, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, archive, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Phar_webPhar, 0, 0, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, alias, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, index, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO(0, f404, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mimetypes, IS_ARRAY, 0, "[]")
	ZEND_ARG_INFO(0, rewrites)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PharData___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "FilesystemIterator::SKIP_DOTS | FilesystemIterator::UNIX_PATHS")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, alias, IS_STRING, 1, "null")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, fileformat, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_PharData___destruct arginfo_class_Phar___destruct

#define arginfo_class_PharData_addEmptyDir arginfo_class_Phar_addEmptyDir

#define arginfo_class_PharData_addFile arginfo_class_Phar_addFile

#define arginfo_class_PharData_addFromString arginfo_class_Phar_addFromString

#define arginfo_class_PharData_buildFromDirectory arginfo_class_Phar_buildFromDirectory

#define arginfo_class_PharData_buildFromIterator arginfo_class_Phar_buildFromIterator

#define arginfo_class_PharData_compressFiles arginfo_class_Phar_compressFiles

#define arginfo_class_PharData_decompressFiles arginfo_class_Phar___destruct

#define arginfo_class_PharData_compress arginfo_class_Phar_compress

#define arginfo_class_PharData_decompress arginfo_class_Phar_decompress

#define arginfo_class_PharData_convertToExecutable arginfo_class_Phar_convertToExecutable

#define arginfo_class_PharData_convertToData arginfo_class_Phar_convertToExecutable

#define arginfo_class_PharData_copy arginfo_class_Phar_copy

#define arginfo_class_PharData_count arginfo_class_Phar_count

#define arginfo_class_PharData_delete arginfo_class_Phar_delete

#define arginfo_class_PharData_delMetadata arginfo_class_Phar___destruct

#define arginfo_class_PharData_extractTo arginfo_class_Phar_extractTo

#define arginfo_class_PharData_getAlias arginfo_class_Phar___destruct

#define arginfo_class_PharData_getPath arginfo_class_Phar___destruct

#define arginfo_class_PharData_getMetadata arginfo_class_Phar___destruct

#define arginfo_class_PharData_getModified arginfo_class_Phar___destruct

#define arginfo_class_PharData_getSignature arginfo_class_Phar___destruct

#define arginfo_class_PharData_getStub arginfo_class_Phar___destruct

#define arginfo_class_PharData_getVersion arginfo_class_Phar___destruct

#define arginfo_class_PharData_hasMetadata arginfo_class_Phar___destruct

#define arginfo_class_PharData_isBuffering arginfo_class_Phar___destruct

#define arginfo_class_PharData_isCompressed arginfo_class_Phar___destruct

#define arginfo_class_PharData_isFileFormat arginfo_class_Phar_isFileFormat

#define arginfo_class_PharData_isWritable arginfo_class_Phar___destruct

#define arginfo_class_PharData_offsetExists arginfo_class_Phar_offsetExists

#define arginfo_class_PharData_offsetGet arginfo_class_Phar_offsetExists

#define arginfo_class_PharData_offsetSet arginfo_class_Phar_offsetSet

#define arginfo_class_PharData_offsetUnset arginfo_class_Phar_offsetExists

#define arginfo_class_PharData_setAlias arginfo_class_Phar_setAlias

#define arginfo_class_PharData_setDefaultStub arginfo_class_Phar_setDefaultStub

#define arginfo_class_PharData_setMetadata arginfo_class_Phar_setMetadata

#define arginfo_class_PharData_setSignatureAlgorithm arginfo_class_Phar_setSignatureAlgorithm

#define arginfo_class_PharData_setStub arginfo_class_Phar_setStub

#define arginfo_class_PharData_startBuffering arginfo_class_Phar___destruct

#define arginfo_class_PharData_stopBuffering arginfo_class_Phar___destruct

#define arginfo_class_PharData_apiVersion arginfo_class_Phar_apiVersion

#define arginfo_class_PharData_canCompress arginfo_class_Phar_canCompress

#define arginfo_class_PharData_canWrite arginfo_class_Phar_canWrite

#define arginfo_class_PharData_createDefaultStub arginfo_class_Phar_createDefaultStub

#define arginfo_class_PharData_getSupportedCompression arginfo_class_Phar_getSupportedCompression

#define arginfo_class_PharData_getSupportedSignatures arginfo_class_Phar_getSupportedCompression

#define arginfo_class_PharData_interceptFileFuncs arginfo_class_Phar_interceptFileFuncs

#define arginfo_class_PharData_isValidPharFilename arginfo_class_Phar_isValidPharFilename

#define arginfo_class_PharData_loadPhar arginfo_class_Phar_loadPhar

#define arginfo_class_PharData_mapPhar arginfo_class_Phar_mapPhar

#define arginfo_class_PharData_running arginfo_class_Phar_running

#define arginfo_class_PharData_mount arginfo_class_Phar_mount

#define arginfo_class_PharData_mungServer arginfo_class_Phar_mungServer

#define arginfo_class_PharData_unlinkArchive arginfo_class_Phar_unlinkArchive

#define arginfo_class_PharData_webPhar arginfo_class_Phar_webPhar

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PharFileInfo___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_PharFileInfo___destruct arginfo_class_Phar___destruct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PharFileInfo_chmod, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, perms, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_PharFileInfo_compress arginfo_class_Phar_compressFiles

#define arginfo_class_PharFileInfo_decompress arginfo_class_Phar___destruct

#define arginfo_class_PharFileInfo_delMetadata arginfo_class_Phar___destruct

#define arginfo_class_PharFileInfo_getCompressedSize arginfo_class_Phar___destruct

#define arginfo_class_PharFileInfo_getCRC32 arginfo_class_Phar___destruct

#define arginfo_class_PharFileInfo_getContent arginfo_class_Phar___destruct

#define arginfo_class_PharFileInfo_getMetadata arginfo_class_Phar___destruct

#define arginfo_class_PharFileInfo_getPharFlags arginfo_class_Phar___destruct

#define arginfo_class_PharFileInfo_hasMetadata arginfo_class_Phar___destruct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PharFileInfo_isCompressed, 0, 0, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, compression_type, "9021976")
ZEND_END_ARG_INFO()

#define arginfo_class_PharFileInfo_isCRCChecked arginfo_class_Phar___destruct

#define arginfo_class_PharFileInfo_setMetadata arginfo_class_Phar_setMetadata


ZEND_METHOD(Phar, __construct);
ZEND_METHOD(Phar, __destruct);
ZEND_METHOD(Phar, addEmptyDir);
ZEND_METHOD(Phar, addFile);
ZEND_METHOD(Phar, addFromString);
ZEND_METHOD(Phar, buildFromDirectory);
ZEND_METHOD(Phar, buildFromIterator);
ZEND_METHOD(Phar, compressFiles);
ZEND_METHOD(Phar, decompressFiles);
ZEND_METHOD(Phar, compress);
ZEND_METHOD(Phar, decompress);
ZEND_METHOD(Phar, convertToExecutable);
ZEND_METHOD(Phar, convertToData);
ZEND_METHOD(Phar, copy);
ZEND_METHOD(Phar, count);
ZEND_METHOD(Phar, delete);
ZEND_METHOD(Phar, delMetadata);
ZEND_METHOD(Phar, extractTo);
ZEND_METHOD(Phar, getAlias);
ZEND_METHOD(Phar, getPath);
ZEND_METHOD(Phar, getMetadata);
ZEND_METHOD(Phar, getModified);
ZEND_METHOD(Phar, getSignature);
ZEND_METHOD(Phar, getStub);
ZEND_METHOD(Phar, getVersion);
ZEND_METHOD(Phar, hasMetadata);
ZEND_METHOD(Phar, isBuffering);
ZEND_METHOD(Phar, isCompressed);
ZEND_METHOD(Phar, isFileFormat);
ZEND_METHOD(Phar, isWritable);
ZEND_METHOD(Phar, offsetExists);
ZEND_METHOD(Phar, offsetGet);
ZEND_METHOD(Phar, offsetSet);
ZEND_METHOD(Phar, offsetUnset);
ZEND_METHOD(Phar, setAlias);
ZEND_METHOD(Phar, setDefaultStub);
ZEND_METHOD(Phar, setMetadata);
ZEND_METHOD(Phar, setSignatureAlgorithm);
ZEND_METHOD(Phar, setStub);
ZEND_METHOD(Phar, startBuffering);
ZEND_METHOD(Phar, stopBuffering);
ZEND_METHOD(Phar, apiVersion);
ZEND_METHOD(Phar, canCompress);
ZEND_METHOD(Phar, canWrite);
ZEND_METHOD(Phar, createDefaultStub);
ZEND_METHOD(Phar, getSupportedCompression);
ZEND_METHOD(Phar, getSupportedSignatures);
ZEND_METHOD(Phar, interceptFileFuncs);
ZEND_METHOD(Phar, isValidPharFilename);
ZEND_METHOD(Phar, loadPhar);
ZEND_METHOD(Phar, mapPhar);
ZEND_METHOD(Phar, running);
ZEND_METHOD(Phar, mount);
ZEND_METHOD(Phar, mungServer);
ZEND_METHOD(Phar, unlinkArchive);
ZEND_METHOD(Phar, webPhar);
ZEND_METHOD(PharFileInfo, __construct);
ZEND_METHOD(PharFileInfo, __destruct);
ZEND_METHOD(PharFileInfo, chmod);
ZEND_METHOD(PharFileInfo, compress);
ZEND_METHOD(PharFileInfo, decompress);
ZEND_METHOD(PharFileInfo, delMetadata);
ZEND_METHOD(PharFileInfo, getCompressedSize);
ZEND_METHOD(PharFileInfo, getCRC32);
ZEND_METHOD(PharFileInfo, getContent);
ZEND_METHOD(PharFileInfo, getMetadata);
ZEND_METHOD(PharFileInfo, getPharFlags);
ZEND_METHOD(PharFileInfo, hasMetadata);
ZEND_METHOD(PharFileInfo, isCompressed);
ZEND_METHOD(PharFileInfo, isCRCChecked);
ZEND_METHOD(PharFileInfo, setMetadata);


static const zend_function_entry class_PharException_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_Phar_methods[] = {
	ZEND_ME(Phar, __construct, arginfo_class_Phar___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, __destruct, arginfo_class_Phar___destruct, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, addEmptyDir, arginfo_class_Phar_addEmptyDir, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, addFile, arginfo_class_Phar_addFile, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, addFromString, arginfo_class_Phar_addFromString, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, buildFromDirectory, arginfo_class_Phar_buildFromDirectory, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, buildFromIterator, arginfo_class_Phar_buildFromIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, compressFiles, arginfo_class_Phar_compressFiles, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, decompressFiles, arginfo_class_Phar_decompressFiles, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, compress, arginfo_class_Phar_compress, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, decompress, arginfo_class_Phar_decompress, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, convertToExecutable, arginfo_class_Phar_convertToExecutable, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, convertToData, arginfo_class_Phar_convertToData, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, copy, arginfo_class_Phar_copy, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, count, arginfo_class_Phar_count, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, delete, arginfo_class_Phar_delete, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, delMetadata, arginfo_class_Phar_delMetadata, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, extractTo, arginfo_class_Phar_extractTo, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, getAlias, arginfo_class_Phar_getAlias, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, getPath, arginfo_class_Phar_getPath, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, getMetadata, arginfo_class_Phar_getMetadata, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, getModified, arginfo_class_Phar_getModified, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, getSignature, arginfo_class_Phar_getSignature, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, getStub, arginfo_class_Phar_getStub, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, getVersion, arginfo_class_Phar_getVersion, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, hasMetadata, arginfo_class_Phar_hasMetadata, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, isBuffering, arginfo_class_Phar_isBuffering, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, isCompressed, arginfo_class_Phar_isCompressed, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, isFileFormat, arginfo_class_Phar_isFileFormat, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, isWritable, arginfo_class_Phar_isWritable, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, offsetExists, arginfo_class_Phar_offsetExists, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, offsetGet, arginfo_class_Phar_offsetGet, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, offsetSet, arginfo_class_Phar_offsetSet, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, offsetUnset, arginfo_class_Phar_offsetUnset, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, setAlias, arginfo_class_Phar_setAlias, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, setDefaultStub, arginfo_class_Phar_setDefaultStub, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, setMetadata, arginfo_class_Phar_setMetadata, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, setSignatureAlgorithm, arginfo_class_Phar_setSignatureAlgorithm, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, setStub, arginfo_class_Phar_setStub, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, startBuffering, arginfo_class_Phar_startBuffering, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, stopBuffering, arginfo_class_Phar_stopBuffering, ZEND_ACC_PUBLIC)
	ZEND_ME(Phar, apiVersion, arginfo_class_Phar_apiVersion, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	ZEND_ME(Phar, canCompress, arginfo_class_Phar_canCompress, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	ZEND_ME(Phar, canWrite, arginfo_class_Phar_canWrite, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	ZEND_ME(Phar, createDefaultStub, arginfo_class_Phar_createDefaultStub, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	ZEND_ME(Phar, getSupportedCompression, arginfo_class_Phar_getSupportedCompression, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	ZEND_ME(Phar, getSupportedSignatures, arginfo_class_Phar_getSupportedSignatures, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	ZEND_ME(Phar, interceptFileFuncs, arginfo_class_Phar_interceptFileFuncs, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	ZEND_ME(Phar, isValidPharFilename, arginfo_class_Phar_isValidPharFilename, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	ZEND_ME(Phar, loadPhar, arginfo_class_Phar_loadPhar, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	ZEND_ME(Phar, mapPhar, arginfo_class_Phar_mapPhar, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	ZEND_ME(Phar, running, arginfo_class_Phar_running, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	ZEND_ME(Phar, mount, arginfo_class_Phar_mount, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	ZEND_ME(Phar, mungServer, arginfo_class_Phar_mungServer, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	ZEND_ME(Phar, unlinkArchive, arginfo_class_Phar_unlinkArchive, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	ZEND_ME(Phar, webPhar, arginfo_class_Phar_webPhar, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	ZEND_FE_END
};


static const zend_function_entry class_PharData_methods[] = {
	ZEND_MALIAS(Phar, __construct, __construct, arginfo_class_PharData___construct, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, __destruct, __destruct, arginfo_class_PharData___destruct, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, addEmptyDir, addEmptyDir, arginfo_class_PharData_addEmptyDir, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, addFile, addFile, arginfo_class_PharData_addFile, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, addFromString, addFromString, arginfo_class_PharData_addFromString, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, buildFromDirectory, buildFromDirectory, arginfo_class_PharData_buildFromDirectory, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, buildFromIterator, buildFromIterator, arginfo_class_PharData_buildFromIterator, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, compressFiles, compressFiles, arginfo_class_PharData_compressFiles, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, decompressFiles, decompressFiles, arginfo_class_PharData_decompressFiles, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, compress, compress, arginfo_class_PharData_compress, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, decompress, decompress, arginfo_class_PharData_decompress, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, convertToExecutable, convertToExecutable, arginfo_class_PharData_convertToExecutable, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, convertToData, convertToData, arginfo_class_PharData_convertToData, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, copy, copy, arginfo_class_PharData_copy, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, count, count, arginfo_class_PharData_count, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, delete, delete, arginfo_class_PharData_delete, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, delMetadata, delMetadata, arginfo_class_PharData_delMetadata, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, extractTo, extractTo, arginfo_class_PharData_extractTo, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, getAlias, getAlias, arginfo_class_PharData_getAlias, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, getPath, getPath, arginfo_class_PharData_getPath, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, getMetadata, getMetadata, arginfo_class_PharData_getMetadata, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, getModified, getModified, arginfo_class_PharData_getModified, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, getSignature, getSignature, arginfo_class_PharData_getSignature, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, getStub, getStub, arginfo_class_PharData_getStub, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, getVersion, getVersion, arginfo_class_PharData_getVersion, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, hasMetadata, hasMetadata, arginfo_class_PharData_hasMetadata, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, isBuffering, isBuffering, arginfo_class_PharData_isBuffering, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, isCompressed, isCompressed, arginfo_class_PharData_isCompressed, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, isFileFormat, isFileFormat, arginfo_class_PharData_isFileFormat, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, isWritable, isWritable, arginfo_class_PharData_isWritable, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, offsetExists, offsetExists, arginfo_class_PharData_offsetExists, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, offsetGet, offsetGet, arginfo_class_PharData_offsetGet, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, offsetSet, offsetSet, arginfo_class_PharData_offsetSet, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, offsetUnset, offsetUnset, arginfo_class_PharData_offsetUnset, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, setAlias, setAlias, arginfo_class_PharData_setAlias, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, setDefaultStub, setDefaultStub, arginfo_class_PharData_setDefaultStub, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, setMetadata, setMetadata, arginfo_class_PharData_setMetadata, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, setSignatureAlgorithm, setSignatureAlgorithm, arginfo_class_PharData_setSignatureAlgorithm, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, setStub, setStub, arginfo_class_PharData_setStub, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, startBuffering, startBuffering, arginfo_class_PharData_startBuffering, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, stopBuffering, stopBuffering, arginfo_class_PharData_stopBuffering, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Phar, apiVersion, apiVersion, arginfo_class_PharData_apiVersion, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	ZEND_MALIAS(Phar, canCompress, canCompress, arginfo_class_PharData_canCompress, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	ZEND_MALIAS(Phar, canWrite, canWrite, arginfo_class_PharData_canWrite, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	ZEND_MALIAS(Phar, createDefaultStub, createDefaultStub, arginfo_class_PharData_createDefaultStub, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	ZEND_MALIAS(Phar, getSupportedCompression, getSupportedCompression, arginfo_class_PharData_getSupportedCompression, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	ZEND_MALIAS(Phar, getSupportedSignatures, getSupportedSignatures, arginfo_class_PharData_getSupportedSignatures, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	ZEND_MALIAS(Phar, interceptFileFuncs, interceptFileFuncs, arginfo_class_PharData_interceptFileFuncs, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	ZEND_MALIAS(Phar, isValidPharFilename, isValidPharFilename, arginfo_class_PharData_isValidPharFilename, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	ZEND_MALIAS(Phar, loadPhar, loadPhar, arginfo_class_PharData_loadPhar, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	ZEND_MALIAS(Phar, mapPhar, mapPhar, arginfo_class_PharData_mapPhar, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	ZEND_MALIAS(Phar, running, running, arginfo_class_PharData_running, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	ZEND_MALIAS(Phar, mount, mount, arginfo_class_PharData_mount, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	ZEND_MALIAS(Phar, mungServer, mungServer, arginfo_class_PharData_mungServer, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	ZEND_MALIAS(Phar, unlinkArchive, unlinkArchive, arginfo_class_PharData_unlinkArchive, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	ZEND_MALIAS(Phar, webPhar, webPhar, arginfo_class_PharData_webPhar, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	ZEND_FE_END
};


static const zend_function_entry class_PharFileInfo_methods[] = {
	ZEND_ME(PharFileInfo, __construct, arginfo_class_PharFileInfo___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(PharFileInfo, __destruct, arginfo_class_PharFileInfo___destruct, ZEND_ACC_PUBLIC)
	ZEND_ME(PharFileInfo, chmod, arginfo_class_PharFileInfo_chmod, ZEND_ACC_PUBLIC)
	ZEND_ME(PharFileInfo, compress, arginfo_class_PharFileInfo_compress, ZEND_ACC_PUBLIC)
	ZEND_ME(PharFileInfo, decompress, arginfo_class_PharFileInfo_decompress, ZEND_ACC_PUBLIC)
	ZEND_ME(PharFileInfo, delMetadata, arginfo_class_PharFileInfo_delMetadata, ZEND_ACC_PUBLIC)
	ZEND_ME(PharFileInfo, getCompressedSize, arginfo_class_PharFileInfo_getCompressedSize, ZEND_ACC_PUBLIC)
	ZEND_ME(PharFileInfo, getCRC32, arginfo_class_PharFileInfo_getCRC32, ZEND_ACC_PUBLIC)
	ZEND_ME(PharFileInfo, getContent, arginfo_class_PharFileInfo_getContent, ZEND_ACC_PUBLIC)
	ZEND_ME(PharFileInfo, getMetadata, arginfo_class_PharFileInfo_getMetadata, ZEND_ACC_PUBLIC)
	ZEND_ME(PharFileInfo, getPharFlags, arginfo_class_PharFileInfo_getPharFlags, ZEND_ACC_PUBLIC)
	ZEND_ME(PharFileInfo, hasMetadata, arginfo_class_PharFileInfo_hasMetadata, ZEND_ACC_PUBLIC)
	ZEND_ME(PharFileInfo, isCompressed, arginfo_class_PharFileInfo_isCompressed, ZEND_ACC_PUBLIC)
	ZEND_ME(PharFileInfo, isCRCChecked, arginfo_class_PharFileInfo_isCRCChecked, ZEND_ACC_PUBLIC)
	ZEND_ME(PharFileInfo, setMetadata, arginfo_class_PharFileInfo_setMetadata, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
