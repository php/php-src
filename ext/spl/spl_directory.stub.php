<?php

/** @generate-function-entries */

class SplFileInfo
{
    public function __construct(string $filename) {}

    /** @return string */
    public function getPath() {}

    /** @return string */
    public function getFilename() {}

    /** @return string */
    public function getExtension() {}

    /** @return string */
    public function getBasename(string $suffix = "") {}

    /** @return string */
    public function getPathname() {}

    /** @return int|false */
    public function getPerms() {}

    /** @return int|false */
    public function getInode() {}

    /** @return int|false */
    public function getSize() {}

    /** @return int|false */
    public function getOwner() {}

    /** @return int|false */
    public function getGroup() {}

    /** @return int|false */
    public function getATime() {}

    /** @return int|false */
    public function getMTime() {}

    /** @return int|false */
    public function getCTime() {}

    /** @return string|false */
    public function getType() {}

    /** @return bool */
    public function isWritable() {}

    /** @return bool */
    public function isReadable() {}

    /** @return bool */
    public function isExecutable() {}

    /** @return bool */
    public function isFile() {}

    /** @return bool */
    public function isDir() {}

    /** @return bool */
    public function isLink() {}

    /** @return string|false */
    public function getLinkTarget() {}

    /** @return string|false */
    public function getRealPath() {}

    /** @return SplFileInfo */
    public function getFileInfo(?string $class = null) {}

    /** @return SplFileInfo|null */
    public function getPathInfo(?string $class = null) {}

    /**
     * @param resource|null $context
     * @return SplFileObject
     */
    public function openFile(string $mode = "r", bool $useIncludePath = false, $context = null) {}

    /** @return void */
    public function setFileClass(string $class = SplFileObject::class) {}

    /** @return void */
    public function setInfoClass(string $class = SplFileInfo::class) {}

    /** @implementation-alias SplFileInfo::getPathname */
    public function __toString(): string {}

    /** @return array */
    public function __debugInfo() {}

    /** @return void */
    final public function _bad_state_ex() {}
}

class DirectoryIterator extends SplFileInfo implements SeekableIterator
{
    public function __construct(string $directory) {}

    /** @return string */
    public function getFilename() {}

    /** @return string */
    public function getExtension() {}

    /** @return string */
    public function getBasename(string $suffix = "") {}

    /** @return bool */
    public function isDot() {}

    /** @return void */
    public function rewind() {}

    /** @return bool */
    public function valid() {}

    /** @return int|false */
    public function key() {}

    /** @return DirectoryIterator */
    public function current() {}

    /** @return void */
    public function next() {}

    /** @return void */
    public function seek(int $offset) {}

    /** @implementation-alias DirectoryIterator::getFilename */
    public function __toString(): string {}
}

class FilesystemIterator extends DirectoryIterator
{
    public function __construct(string $directory, int $flags = FilesystemIterator::KEY_AS_PATHNAME | FilesystemIterator::CURRENT_AS_FILEINFO | FilesystemIterator::SKIP_DOTS) {}

    /** @return void */
    public function rewind() {}

    /** @return string */
    public function key() {}

    /** @return string|SplFileInfo|FilesystemIterator */
    public function current() {}

    /** @return int */
    public function getFlags() {}

    /** @return void */
    public function setFlags(int $flags) {}
}

class RecursiveDirectoryIterator extends FilesystemIterator implements RecursiveIterator
{
    public function __construct(string $directory, int $flags = FilesystemIterator::KEY_AS_PATHNAME | FilesystemIterator::CURRENT_AS_FILEINFO) {}

    /** @return bool */
    public function hasChildren(bool $allowLinks = false) {}

    /** @return RecursiveDirectoryIterator */
    public function getChildren() {}

    /** @return string */
    public function getSubPath() {}

    /** @return string */
    public function getSubPathname() {}
}

#ifdef HAVE_GLOB
class GlobIterator extends FilesystemIterator implements Countable
{
    public function __construct(string $pattern, int $flags = FilesystemIterator::KEY_AS_PATHNAME | FilesystemIterator::CURRENT_AS_FILEINFO) {}

    /** @return int */
    public function count() {}
}
#endif

class SplFileObject extends SplFileInfo implements RecursiveIterator, SeekableIterator
{
    /** @param resource|null $context */
    public function __construct(string $filename, string $mode = "r", bool $useIncludePath = false, $context = null) {}

    /** @return void */
    public function rewind() {}

    /** @return bool */
    public function eof() {}

    /** @return bool */
    public function valid() {}

    /** @return string */
    public function fgets() {}

    /** @return string|false */
    public function fread(int $length) {}

    /** @return array|false */
    public function fgetcsv(string $separator = ",", string $enclosure = "\"", string $escape = "\\") {}

    /** @return int|false */
    public function fputcsv(array $fields, string $separator = ",", string $enclosure = "\"", string $escape = "\\") {}

    /** @return void */
    public function setCsvControl(string $separator = ",", string $enclosure = "\"", string $escape = "\\") {}

    /** @return array */
    public function getCsvControl() {}

    /**
     * @param int $wouldBlock
     * @return bool
     */
    public function flock(int $operation, &$wouldBlock = null) {}

    /** @return bool */
    public function fflush() {}

    /** @return int|false */
    public function ftell() {}

    /** @return int */
    public function fseek(int $offset, int $whence = SEEK_SET) {}

    /** @return string|false */
    public function fgetc() {}

    /** @return int */
    public function fpassthru() {}

    /** @return array|int|null */
    public function fscanf(string $format, mixed &...$vars) {}

    /** @return int|false */
    public function fwrite(string $data, int $length = 0) {}

    /** @return array */
    public function fstat() {}

    /** @return bool */
    public function ftruncate(int $size) {}

    /** @return string|array|false */
    public function current() {}

    /** @return int */
    public function key() {}

    /** @return void */
    public function next() {}

    /** @return void */
    public function setFlags(int $flags) {}

    /** @return int */
    public function getFlags() {}

    /** @return void */
    public function setMaxLineLen(int $maxLength) {}

    /** @return int */
    public function getMaxLineLen() {}

    /** @return bool */
    public function hasChildren() {}

    /** @return RecursiveIterator|null */
    public function getChildren() {}

    /** @return void */
    public function seek(int $line) {}

    /**
     * @return string
     * @alias SplFileObject::fgets
     */
    public function getCurrentLine() {}

    /** @implementation-alias SplFileObject::fgets */
    public function __toString(): string {}
}

class SplTempFileObject extends SplFileObject
{
    public function __construct(int $maxMemory = 2 * 1024 * 1024) {}
}
