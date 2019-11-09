<?php

class Phar extends RecursiveDirectoryIterator implements Countable, ArrayAccess
{
    public function __construct(string $filename, int $flags = FilesystemIterator::SKIP_DOTS|FilesystemIterator::UNIX_PATHS, ?string $alias = null) {}

    public function __destruct() {}

    /** @return void */
    public function addEmptyDir(string $dirname) {}

    /** @return void */
    public function addFile(string $filename, string $localname = UNKNOWN) {}

    /** @return void */
    public function addFromString(string $localname, string $contents) {}

    /** @return array|false */
    public function buildFromDirectory(string $base_dir, string $regex = UNKNOWN) {}

    /** @return array|false */
    public function buildFromIterator(Traversable $iterator, string $base_directory = UNKNOWN) {}

    /** @return void */
    public function compressFiles(int $compression_type) {}

    /** @return bool */
    public function decompressFiles() {}

    /** @return ?Phar */
    public function compress(int $compression_type, string $file_ext = UNKNOWN) {}

    /** @return ?Phar */
    public function decompress(string $file_ext = UNKNOWN) {}

    /** @return ?Phar */
    public function convertToExecutable(int $format = 9021976, int $compression_type = 9021976, string $file_ext = UNKNOWN) {}

    /** @return ?Phar */
    public function convertToData(int $format = 9021976, int $compression_type = 9021976, string $file_ext = UNKNOWN) {}

    /** @return bool */
    public function copy(string $newfile, string $oldfile) {}

    /** @return int */
    public function count(int $mode = UNKNOWN) {}

    /** @return bool */
    public function delete(string $entry) {}

    /** @return bool */
    public function delMetadata() {}

    /** @return bool */
    public function extractTo(string $pathto, $files = null, bool $overwrite = false) {}

    /** @return ?string */
    public function getAlias() {}

    /** @return string */
    public function getPath() {}

    /** @return mixed */
    public function getMetadata() {}

    /** @return bool */
    public function getModified() {}

    /** @return array|false */
    public function getSignature() {}

    /** @return string */
    public function getStub() {}

    /** @return string */
    public function getVersion() {}

    /** @return bool */
    public function hasMetadata() {}

    /** @return bool */
    public function isBuffering() {}

    /** @return int|false */
    public function isCompressed() {}

    /** @return bool */
    public function isFileFormat(int $fileformat) {}

    /** @return bool */
    public function isWritable() {}

    /**
     * @param string $entry
     * @return bool
     */
    public function offsetExists($entry) {}

    /** 
     * @param string $entry
     * @return PharFileInfo
     */
    public function offsetGet($entry) {}

    /**
     * @param string $entry
     * @param resource|string $value
     * @return void
     */
    public function offsetSet($entry, $value) {}

    /**
     * @param string $entry
     * @return bool
     */
    public function offsetUnset($entry) {}

    /** @return bool */
    public function setAlias(string $alias) {}

    /** @return bool */
    public function setDefaultStub(?string $index = null, string $webindex = UNKNOWN) {}

    /**
     * @param mixed $metadata
     * @return void
     */
    public function setMetadata($metadata) {}

    /** @return void */
    public function setSignatureAlgorithm(int $algorithm, string $privatekey = UNKNOWN) {}

    /** 
     * @param resource $newstub
     * @return bool
     */
    public function setStub($newstub, $maxlen = -1) {}

    /** @return void */
    public function startBuffering() {}

    /** @return void */
    public function stopBuffering() {}

    /** @return string */
    final public static function apiVersion() {}

    /** @return bool */
    final public static function canCompress(int $method = 0) {}

    /** @return bool */
    final public static function canWrite() {}

    /** @return string */
    final public static function createDefaultStub(string $index = UNKNOWN, string $webindex = UNKNOWN) {}

    /** @return array */
    final public static function getSupportedCompression() {}

    /** @return array */
    final public static function getSupportedSignatures() {}

    /** @return void */
    final public static function interceptFileFuncs() {}

    /** @return bool */
    final public static function isValidPharFilename(string $filename, bool $executable = true) {}

    /** @return bool */
    final public static function loadPhar(string $filename, ?string $alias = null) {}

    /** @return bool */
    final public static function mapPhar(?string $alias = null, int $offset = 0) {}

    /** @return string */
    final public static function running(bool $retphar = true) {}

    /** @return void */
    final public static function mount(string $inphar, string $externalfile) {}

    /** @return void */
    final public static function mungServer(array $munglist) {}

    /** @return bool */
    final public static function unlinkArchive(string $archive) {}

    /** @return void */
    final public static function webPhar(?string $alias = null, ?string $index = null, string $f404 = UNKNOWN, array $mimetypes = [], $rewrites = UNKNOWN) {}
}

class PharData extends RecursiveDirectoryIterator implements Countable, ArrayAccess
{
    public function __construct(string $filename, int $flags = FilesystemIterator::SKIP_DOTS|FilesystemIterator::UNIX_PATHS, ?string $alias = null, $fileformat = 0) {}

    public function __destruct() {}

    /** @return void */
    public function addEmptyDir(string $dirname) {}

    /** @return void */
    public function addFile(string $filename, string $localname = UNKNOWN) {}

    /** @return void */
    public function addFromString(string $localname, string $contents) {}

    /** @return array|false */
    public function buildFromDirectory(string $base_dir, string $regex = UNKNOWN) {}

    /** @return array|false */
    public function buildFromIterator(Traversable $iterator, string $base_directory = UNKNOWN) {}

    /** @return void */
    public function compressFiles(int $compression_type) {}

    /** @return bool */
    public function decompressFiles() {}

    /** @return ?Phar */
    public function compress(int $compression_type, string $file_ext = UNKNOWN) {}

    /** @return ?Phar */
    public function decompress(string $file_ext = UNKNOWN) {}

    /** @return ?Phar */
    public function convertToExecutable(int $format = 9021976, int $compression_type = 9021976, string $file_ext = UNKNOWN) {}

    /** @return ?Phar */
    public function convertToData(int $format = 9021976, int $compression_type = 9021976, string $file_ext = UNKNOWN) {}

    /** @return bool */
    public function copy(string $newfile, string $oldfile) {}

    /** @return int */
    public function count(int $mode = UNKNOWN) {}

    /** @return bool */
    public function delete(string $entry) {}

    /** @return bool */
    public function delMetadata() {}

    /** @return bool */
    public function extractTo(string $pathto, $files = null, bool $overwrite = false) {}

    /** @return ?string */
    public function getAlias() {}

    /** @return string */
    public function getPath() {}

    /** @return mixed */
    public function getMetadata() {}

    /** @return bool */
    public function getModified() {}

    /** @return array|false */
    public function getSignature() {}

    /** @return string */
    public function getStub() {}

    /** @return string */
    public function getVersion() {}

    /** @return bool */
    public function hasMetadata() {}

    /** @return bool */
    public function isBuffering() {}

    /** @return int|false */
    public function isCompressed() {}

    /** @return bool */
    public function isFileFormat(int $fileformat) {}

    /** @return bool */
    public function isWritable() {}

    /**
     * @param string $entry
     * @return bool
     */
    public function offsetExists($entry) {}

    /**
     * @param string $entry
     * @return PharFileInfo
     */
    public function offsetGet($entry) {}

    /**
     * @param string $entry
     * @param resource|string $value
     * @return void
     */
    public function offsetSet($entry, $value) {}

    /**
     * @param string $entry
     * @return bool
     */
    public function offsetUnset($entry) {}

    /** @return bool */
    public function setAlias(string $alias) {}

    /** @return bool */
    public function setDefaultStub(?string $index = null, string $webindex = UNKNOWN) {}

    /**
     * @param mixed $metadata
     * @return void
     */
    public function setMetadata($metadata) {}

    /** @return void */
    public function setSignatureAlgorithm(int $algorithm, string $privatekey = UNKNOWN) {}

    /** 
     * @param resource $newstub
     * @return bool
     */
    public function setStub($newstub, $maxlen = -1) {}

    /** @return void */
    public function startBuffering() {}

    /** @return void */
    public function stopBuffering() {}

    /** @return string */
    final public static function apiVersion() {}

    /** @return bool */
    final public static function canCompress(int $method = 0) {}

    /** @return bool */
    final public static function canWrite() {}

    /** @return string */
    final public static function createDefaultStub(string $index = UNKNOWN, string $webindex = UNKNOWN) {}

    /** @return array */
    final public static function getSupportedCompression() {}

    /** @return array */
    final public static function getSupportedSignatures() {}

    /** @return void */
    final public static function interceptFileFuncs() {}

    /** @return bool */
    final public static function isValidPharFilename(string $filename, bool $executable = true) {}

    /** @return bool */
    final public static function loadPhar(string $filename, ?string $alias = null) {}

    /** @return bool */
    final public static function mapPhar(?string $alias = null, int $offset = 0) {}

    /** @return string */
    final public static function running(bool $retphar = true) {}

    /** @return void */
    final public static function mount(string $inphar, string $externalfile) {}

    /** @return void */
    final public static function mungServer(array $munglist) {}

    /** @return bool */
    final public static function unlinkArchive(string $archive) {}

    /** @return void */
    final public static function webPhar(?string $alias = null, ?string $index = null, string $f404 = UNKNOWN, array $mimetypes = [], $rewrites = UNKNOWN) {}
}

class PharFileInfo extends SplFileInfo
{
    public function __construct(string $filename) {}

    public function __destruct() {}

    /** @return void */
    public function chmod(int $perms) {}

    /** @return bool */
    public function compress(int $compression_type) {}

    /** @return bool */
    public function decompress() {}

    /** @return bool */
    public function delMetadata() {}

    /** @return int */
    public function getCompressedSize() {}

    /** @return int */
    public function getCRC32() {}

    /** @return string */
    public function getContent() {}

    public function getMetadata() {}

    /** @return int */
    public function getPharFlags() {}

    /** @return bool */
    public function hasMetadata() {}

    /** @return bool */
    public function isCompressed($compression_type = 9021976) {}

    /** @return bool */
    public function isCRCChecked() {}

    /** @return void */
    public function setMetadata($metadata) {}
}
