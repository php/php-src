<?php

/** @generate-function-entries */

class SplFileInfo
{
    public function __construct(string $file_name) {}

    /** @return string */
    public function getPath() {}

    /** @return string */
    public function getFilename() {}

    /** @return string */
    public function getExtension() {}

    /** @return string */
    public function getBasename(string $suffix = "") {}

    /** @return string|false */
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
    public function getFileInfo(string $class_name = UNKNOWN) {}

    /** @return SplFileInfo|null */
    public function getPathInfo(string $class_name = UNKNOWN) {}

    /**
     * @param resource $context
     * @return SplFileObject
     */
    public function openFile(string $open_mode = 'r', bool $use_include_path = false, $context = UNKNOWN) {}

    /** @return void */
    public function setFileClass(string $class_name = SplFileObject::class) {}

    /** @return void */
    public function setInfoClass(string $class_name = SplFileInfo::class) {}

    /** @alias SplFileInfo::getPathname */
    public function __toString(): string {}

    /** @return array */
    public function __debugInfo() {}

    /** @return void */
    final public function _bad_state_ex() {}
}

class DirectoryIterator extends SplFileInfo implements SeekableIterator
{
    public function __construct(string $path) {}

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
    public function seek(int $position) {}

    /** @alias DirectoryIterator::getFilename */
    public function __toString(): string {}
}

class FilesystemIterator extends DirectoryIterator
{
    public function __construct(string $path, int $flags = FilesystemIterator::KEY_AS_PATHNAME | FilesystemIterator::CURRENT_AS_FILEINFO | FilesystemIterator::SKIP_DOTS) {}

    /** @return void */
    public function rewind() {}

    /** @return string */
    public function key() {}

    /** @return string|SplFileInfo|self */
    public function current() {}

    /** @return int */
    public function getFlags() {}

    /** @return void */
    public function setFlags(int $flags) {}
}

class RecursiveDirectoryIterator extends FilesystemIterator implements RecursiveIterator
{
    public function __construct(string $path, int $flags = FilesystemIterator::KEY_AS_PATHNAME | FilesystemIterator::CURRENT_AS_FILEINFO) {}

    /** @return bool */
    public function hasChildren(bool $allow_links = false) {}

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
    public function __construct(string $path, int $flags = FilesystemIterator::KEY_AS_PATHNAME | FilesystemIterator::CURRENT_AS_FILEINFO) {}

    /** @return int */
    public function count() {}
}
#endif

class SplFileObject extends SplFileInfo implements RecursiveIterator, SeekableIterator
{
    /** @param resource|null $context */
    public function __construct(string $file_name, string $open_mode = 'r', bool $use_include_path = false, $context = null) {}

    /** @return void */
    public function rewind() {}

    /** @return bool */
    public function eof() {}

    /** @return bool */
    public function valid() {}

    /** @return string|false */
    public function fgets() {}

    /** @return string|false */
    public function fread(int $length) {}

    /** @return array|false */
    public function fgetcsv(string $delimiter = ",", string $enclosure = '"', $escape = "\\") {}

    /** @return int|false */
    public function fputcsv(array $fields, string $delimiter = ',', string $enclosure = '"', string $escape = "\\") {}

    /** @return null|false */
    public function setCsvControl(string $delimiter = ",", string $enclosure = "\"", string $escape = "\\") {}

    /** @return array */
    public function getCsvControl() {}

    /** @return bool */
    public function flock(int $operation, &$wouldblock = null) {}

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

    /**
     * @param string $format
     * @return array|int
     */
    public function fscanf(string $format, &...$params) {}

    /** @return int|false */
    public function fwrite(string $str, int $length = 0) {}

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
    public function setMaxLineLen(int $max_len) {}

    /** @return int */
    public function getMaxLineLen() {}

    /** @return bool */
    public function hasChildren() {}

    /** @return null */
    public function getChildren() {}

    /** @return void */
    public function seek(int $line_pos) {}

    /**
     * @return string|false
     * @alias SplFileObject::fgets
     */
    public function getCurrentLine() {}

    /** @alias SplFileObject::fgets */
    public function __toString(): string {}
}

class SplTempFileObject extends SplFileObject
{
    public function __construct(int $max_memory = 2 * 1024 * 1024) {}
}
