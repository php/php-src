<?php

/** @generate-function-entries */

class PharException extends Exception
{
}

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

    /** @return Phar|null */
    public function compress(int $compression_type, string $file_ext = UNKNOWN) {}

    /** @return Phar|null */
    public function decompress(string $file_ext = UNKNOWN) {}

    /** @return Phar|null */
    public function convertToExecutable(int $format = 9021976, int $compression_type = 9021976, string $file_ext = UNKNOWN) {}

    /** @return Phar|null */
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

    /** @return string|null */
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

    final public static function apiVersion(): string {}

    final public static function canCompress(int $method = 0): bool {}

    final public static function canWrite(): bool {}

    final public static function createDefaultStub(
        string $index = UNKNOWN, string $webindex = UNKNOWN): string {}

    final public static function getSupportedCompression(): array {}

    final public static function getSupportedSignatures(): array {}

    final public static function interceptFileFuncs(): void {}

    final public static function isValidPharFilename(
        string $filename, bool $executable = true): bool {}

    final public static function loadPhar(string $filename, ?string $alias = null): bool {}

    final public static function mapPhar(?string $alias = null, int $offset = 0): bool {}

    final public static function running(bool $retphar = true): string {}

    final public static function mount(string $inphar, string $externalfile): void {}

    final public static function mungServer(array $munglist): void {}

    final public static function unlinkArchive(string $archive): bool {}

    final public static function webPhar(
        ?string $alias = null, ?string $index = null, string $f404 = UNKNOWN,
        array $mimetypes = [], $rewrites = UNKNOWN): void {}
}

class PharData extends RecursiveDirectoryIterator implements Countable, ArrayAccess
{
    /** @alias Phar::__construct */
    public function __construct(string $filename, int $flags = FilesystemIterator::SKIP_DOTS|FilesystemIterator::UNIX_PATHS, ?string $alias = null, $fileformat = 0) {}

    /** @alias Phar::__destruct */
    public function __destruct() {}

    /**
     * @return void
     * @alias Phar::addEmptyDir
     */
    public function addEmptyDir(string $dirname) {}

    /**
     * @return void
     * @alias Phar::addFile
     */
    public function addFile(string $filename, string $localname = UNKNOWN) {}

    /**
     * @return void
     * @alias Phar::addFromString
     */
    public function addFromString(string $localname, string $contents) {}

    /**
     * @return array|false
     * @alias Phar::buildFromDirectory
     */
    public function buildFromDirectory(string $base_dir, string $regex = UNKNOWN) {}

    /**
     * @return array|false
     * @alias Phar::buildFromIterator
     */
    public function buildFromIterator(Traversable $iterator, string $base_directory = UNKNOWN) {}

    /**
     * @return void
     * @alias Phar::compressFiles
     */
    public function compressFiles(int $compression_type) {}

    /**
     * @return bool
     * @alias Phar::decompressFiles
     */
    public function decompressFiles() {}

    /**
     * @return Phar|null
     * @alias Phar::compress
     */
    public function compress(int $compression_type, string $file_ext = UNKNOWN) {}

    /**
     * @return Phar|null
     * @alias Phar::decompress
     */
    public function decompress(string $file_ext = UNKNOWN) {}

    /**
     * @return Phar|null
     * @alias Phar::convertToExecutable
     */
    public function convertToExecutable(int $format = 9021976, int $compression_type = 9021976, string $file_ext = UNKNOWN) {}

    /**
     * @return Phar|null
     * @alias Phar::convertToData
     */
    public function convertToData(int $format = 9021976, int $compression_type = 9021976, string $file_ext = UNKNOWN) {}

    /**
     * @return bool
     * @alias Phar::copy
     */
    public function copy(string $newfile, string $oldfile) {}

    /**
     * @return int
     * @alias Phar::count
     */
    public function count(int $mode = UNKNOWN) {}

    /**
     * @return bool
     * @alias Phar::delete
     */
    public function delete(string $entry) {}

    /**
     * @return bool
     * @alias Phar::delMetadata
     */
    public function delMetadata() {}

    /**
     * @return bool
     * @alias Phar::extractTo
     */
    public function extractTo(string $pathto, $files = null, bool $overwrite = false) {}

    /**
     * @return string|null
     * @alias Phar::getAlias
     */
    public function getAlias() {}

    /**
     * @return string
     * @alias Phar::getPath
     */
    public function getPath() {}

    /**
     * @return mixed
     * @alias Phar::getMetadata
     */
    public function getMetadata() {}

    /**
     * @return bool
     * @alias Phar::getModified
     */
    public function getModified() {}

    /**
     * @return array|false
     * @alias Phar::getSignature
     */
    public function getSignature() {}

    /**
     * @return string
     * @alias Phar::getStub
     */
    public function getStub() {}

    /**
     * @return string
     * @alias Phar::getVersion
     */
    public function getVersion() {}

    /**
     * @return bool
     * @alias Phar::hasMetadata
     */
    public function hasMetadata() {}

    /**
     * @return bool
     * @alias Phar::isBuffering
     */
    public function isBuffering() {}

    /**
     * @return int|false
     * @alias Phar::isCompressed
     */
    public function isCompressed() {}

    /**
     * @return bool
     * @alias Phar::isFileFormat
     */
    public function isFileFormat(int $fileformat) {}

    /**
     * @return bool
     * @alias Phar::isWritable
     */
    public function isWritable() {}

    /**
     * @param string $entry
     * @return bool
     * @alias Phar::offsetExists
     */
    public function offsetExists($entry) {}

    /**
     * @param string $entry
     * @return PharFileInfo
     * @alias Phar::offsetGet
     */
    public function offsetGet($entry) {}

    /**
     * @param string $entry
     * @param resource|string $value
     * @return void
     * @alias Phar::offsetSet
     */
    public function offsetSet($entry, $value) {}

    /**
     * @param string $entry
     * @return bool
     * @alias Phar::offsetUnset
     */
    public function offsetUnset($entry) {}

    /**
     * @return bool
     * @alias Phar::setAlias
     */
    public function setAlias(string $alias) {}

    /**
     * @return bool
     * @alias Phar::setDefaultStub
     */
    public function setDefaultStub(?string $index = null, string $webindex = UNKNOWN) {}

    /**
     * @param mixed $metadata
     * @return void
     * @alias Phar::setMetadata
     */
    public function setMetadata($metadata) {}

    /**
     * @return void
     * @alias Phar::setSignatureAlgorithm
     */
    public function setSignatureAlgorithm(int $algorithm, string $privatekey = UNKNOWN) {}

    /**
     * @param resource $newstub
     * @return bool
     * @alias Phar::setStub
     */
    public function setStub($newstub, $maxlen = -1) {}

    /**
     * @return void
     * @alias Phar::startBuffering
     */
    public function startBuffering() {}

    /**
     * @return void
     * @alias Phar::stopBuffering
     */
    public function stopBuffering() {}

    /** @alias Phar::apiVersion */
    final public static function apiVersion(): string {}

    /** @alias Phar::canCompress */
    final public static function canCompress(int $method = 0): bool {}

    /** @alias Phar::canWrite */
    final public static function canWrite(): bool {}

    /** @alias Phar::createDefaultStub */
    final public static function createDefaultStub(
        string $index = UNKNOWN, string $webindex = UNKNOWN): string {}

    /** @alias Phar::getSupportedCompression */
    final public static function getSupportedCompression(): array {}

    /** @alias Phar::getSupportedSignatures */
    final public static function getSupportedSignatures(): array {}

    /** @alias Phar::interceptFileFuncs */
    final public static function interceptFileFuncs(): void {}

    /** @alias Phar::isValidPharFilename */
    final public static function isValidPharFilename(
        string $filename, bool $executable = true): bool {}

    /** @alias Phar::loadPhar */
    final public static function loadPhar(string $filename, ?string $alias = null): bool {}

    /** @alias Phar::mapPhar */
    final public static function mapPhar(?string $alias = null, int $offset = 0): bool {}

    /** @alias Phar::running */
    final public static function running(bool $retphar = true): string {}

    /** @alias Phar::mount */
    final public static function mount(string $inphar, string $externalfile): void {}

    /** @alias Phar::mungServer */
    final public static function mungServer(array $munglist): void {}

    /** @alias Phar::unlinkArchive */
    final public static function unlinkArchive(string $archive): bool {}

    /** @alias Phar::webPhar */
    final public static function webPhar(
        ?string $alias = null, ?string $index = null, string $f404 = UNKNOWN,
        array $mimetypes = [], $rewrites = UNKNOWN): void {}
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

    /** @return mixed */
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
