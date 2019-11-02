/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Phar___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, alias, IS_STRING, 1)
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
	ZEND_ARG_TYPE_INFO(0, format, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, compression_type, IS_LONG, 0)
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
	ZEND_ARG_INFO(0, files)
	ZEND_ARG_TYPE_INFO(0, overwrite, _IS_BOOL, 0)
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
	ZEND_ARG_TYPE_INFO(0, index, IS_STRING, 1)
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
	ZEND_ARG_INFO(0, maxlen)
ZEND_END_ARG_INFO()

#define arginfo_class_Phar_startBuffering arginfo_class_Phar___destruct

#define arginfo_class_Phar_stopBuffering arginfo_class_Phar___destruct

#define arginfo_class_Phar_apiVersion arginfo_class_Phar___destruct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Phar_canCompress, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, method, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Phar_canWrite arginfo_class_Phar___destruct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Phar_createDefaultStub, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, webindex, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Phar_getSupportedCompression arginfo_class_Phar___destruct

#define arginfo_class_Phar_getSupportedSignatures arginfo_class_Phar___destruct

#define arginfo_class_Phar_interceptFileFuncs arginfo_class_Phar___destruct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Phar_isValidPharFilename, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, executable, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Phar_loadPhar, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, alias, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Phar_mapPhar, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, alias, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Phar_running, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, retphar, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Phar_mount, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, inphar, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, externalfile, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Phar_mungServer, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, munglist, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Phar_unlinkArchive, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, archive, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Phar_webPhar, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, alias, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, index, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, f404, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, mimetypes, IS_ARRAY, 0)
	ZEND_ARG_INFO(0, rewrites)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PharData___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, alias, IS_STRING, 1)
	ZEND_ARG_INFO(0, fileformat)
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

#define arginfo_class_PharData_apiVersion arginfo_class_Phar___destruct

#define arginfo_class_PharData_canCompress arginfo_class_Phar_canCompress

#define arginfo_class_PharData_canWrite arginfo_class_Phar___destruct

#define arginfo_class_PharData_createDefaultStub arginfo_class_Phar_createDefaultStub

#define arginfo_class_PharData_getSupportedCompression arginfo_class_Phar___destruct

#define arginfo_class_PharData_getSupportedSignatures arginfo_class_Phar___destruct

#define arginfo_class_PharData_interceptFileFuncs arginfo_class_Phar___destruct

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
	ZEND_ARG_INFO(0, compression_type)
ZEND_END_ARG_INFO()

#define arginfo_class_PharFileInfo_isCRCChecked arginfo_class_Phar___destruct

#define arginfo_class_PharFileInfo_setMetadata arginfo_class_Phar_setMetadata
