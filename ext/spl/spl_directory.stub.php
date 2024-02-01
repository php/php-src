<?php

/** @generate-class-entries */

/** @not-serializable */
class SplFileInfo implements Stringable
{
    public function __construct(string $filename) {}

    /** @tentative-return-type */
    public function getPath(): string {}

    /** @tentative-return-type */
    public function getFilename(): string {}

    /** @tentative-return-type */
    public function getExtension(): string {}

    /** @tentative-return-type */
    public function getBasename(string $suffix = ""): string {}

    /** @tentative-return-type */
    public function getPathname(): string {}

    /** @tentative-return-type */
    public function getPerms(): int|false {}

    /** @tentative-return-type */
    public function getInode(): int|false {}

    /** @tentative-return-type */
    public function getSize(): int|false {}

    /** @tentative-return-type */
    public function getOwner(): int|false {}

    /** @tentative-return-type */
    public function getGroup(): int|false {}

    /** @tentative-return-type */
    public function getATime(): int|false {}

    /** @tentative-return-type */
    public function getMTime(): int|false {}

    /** @tentative-return-type */
    public function getCTime(): int|false {}

    /** @tentative-return-type */
    public function getType(): string|false {}

    /** @tentative-return-type */
    public function isWritable(): bool {}

    /** @tentative-return-type */
    public function isReadable(): bool {}

    /** @tentative-return-type */
    public function isExecutable(): bool {}

    /** @tentative-return-type */
    public function isFile(): bool {}

    /** @tentative-return-type */
    public function isDir(): bool {}

    /** @tentative-return-type */
    public function isLink(): bool {}

    /** @tentative-return-type */
    public function getLinkTarget(): string|false {}

    /** @tentative-return-type */
    public function getRealPath(): string|false {}

    /** @tentative-return-type */
    public function getFileInfo(?string $class = null): SplFileInfo {}

    /** @tentative-return-type */
    public function getPathInfo(?string $class = null): ?SplFileInfo {}

    /**
     * @param resource|null $context
     * @tentative-return-type
     */
    public function openFile(string $mode = "r", bool $useIncludePath = false, $context = null): SplFileObject {}

    /** @tentative-return-type */
    public function setFileClass(string $class = SplFileObject::class): void {}

    /** @tentative-return-type */
    public function setInfoClass(string $class = SplFileInfo::class): void {}

    /** @implementation-alias SplFileInfo::getPathname */
    public function __toString(): string {}

    /** @tentative-return-type */
    public function __debugInfo(): array {}

    /**
     * @deprecated
     * @tentative-return-type
     */
    final public function _bad_state_ex(): void {}
}

class DirectoryIterator extends SplFileInfo implements SeekableIterator
{
    public function __construct(string $directory) {}

    /** @tentative-return-type */
    public function getFilename(): string {}

    /** @tentative-return-type */
    public function getExtension(): string {}

    /** @tentative-return-type */
    public function getBasename(string $suffix = ""): string {}

    /** @tentative-return-type */
    public function isDot(): bool {}

    /** @tentative-return-type */
    public function rewind(): void {}

    /** @tentative-return-type */
    public function valid(): bool {}

    /**
     * @tentative-return-type
     * @return int
     */
    public function key(): mixed {} // TODO change return type to string

    /**
     * @tentative-return-type
     * @return DirectoryIterator
     */
    public function current(): mixed {} // TODO narrow return type

    /** @tentative-return-type */
    public function next(): void {}

    /** @tentative-return-type */
    public function seek(int $offset): void {}

    /** @implementation-alias DirectoryIterator::getFilename */
    public function __toString(): string {}
}

class FilesystemIterator extends DirectoryIterator
{
    /**
     * @var int
     * @cvalue SPL_FILE_DIR_CURRENT_MODE_MASK
     */
    public const CURRENT_MODE_MASK = UNKNOWN;
    /**
     * @var int
     * @cvalue SPL_FILE_DIR_CURRENT_AS_PATHNAME
     */
    public const CURRENT_AS_PATHNAME = UNKNOWN;
    /**
     * @var int
     * @cvalue SPL_FILE_DIR_CURRENT_AS_FILEINFO
     */
    public const CURRENT_AS_FILEINFO = UNKNOWN;
    /**
     * @var int
     * @cvalue SPL_FILE_DIR_CURRENT_AS_SELF
     */
    public const CURRENT_AS_SELF = UNKNOWN;
    /**
     * @var int
     * @cvalue SPL_FILE_DIR_KEY_MODE_MASK
     */
    public const KEY_MODE_MASK = UNKNOWN;
    /**
     * @var int
     * @cvalue SPL_FILE_DIR_KEY_AS_PATHNAME
     */
    public const KEY_AS_PATHNAME = UNKNOWN;
    /**
     * @var int
     * @cvalue SPL_FILE_DIR_FOLLOW_SYMLINKS
     */
    public const FOLLOW_SYMLINKS = UNKNOWN;
    /**
     * @var int
     * @cvalue SPL_FILE_DIR_KEY_AS_FILENAME
     */
    public const KEY_AS_FILENAME = UNKNOWN;
    /**
     * @var int
     * @cvalue SPL_FILE_NEW_CURRENT_AND_KEY
     */
    public const NEW_CURRENT_AND_KEY = UNKNOWN;
    /**
     * @var int
     * @cvalue SPL_FILE_DIR_OTHERS_MASK
     */
    public const OTHER_MODE_MASK = UNKNOWN;
    /**
     * @var int
     * @cvalue SPL_FILE_DIR_SKIPDOTS
     */
    public const SKIP_DOTS = UNKNOWN;
    /**
     * @var int
     * @cvalue SPL_FILE_DIR_UNIXPATHS
     */
    public const UNIX_PATHS = UNKNOWN;

    public function __construct(string $directory, int $flags = FilesystemIterator::KEY_AS_PATHNAME | FilesystemIterator::CURRENT_AS_FILEINFO | FilesystemIterator::SKIP_DOTS) {}

    /** @tentative-return-type */
    public function rewind(): void {}

    /** @tentative-return-type */
    public function key(): string {}

    /** @tentative-return-type */
    public function current(): string|SplFileInfo|FilesystemIterator {}

    /** @tentative-return-type */
    public function getFlags(): int {}

    /** @tentative-return-type */
    public function setFlags(int $flags): void {}
}

class RecursiveDirectoryIterator extends FilesystemIterator implements RecursiveIterator
{
    public function __construct(string $directory, int $flags = FilesystemIterator::KEY_AS_PATHNAME | FilesystemIterator::CURRENT_AS_FILEINFO) {}

    /** @tentative-return-type */
    public function hasChildren(bool $allowLinks = false): bool {}

    /** @tentative-return-type */
    public function getChildren(): RecursiveDirectoryIterator {}

    /** @tentative-return-type */
    public function getSubPath(): string {}

    /** @tentative-return-type */
    public function getSubPathname(): string {}
}

#ifdef HAVE_GLOB
class GlobIterator extends FilesystemIterator implements Countable
{
    public function __construct(string $pattern, int $flags = FilesystemIterator::KEY_AS_PATHNAME | FilesystemIterator::CURRENT_AS_FILEINFO) {}

    /** @tentative-return-type */
    public function count(): int {}
}
#endif

class SplFileObject extends SplFileInfo implements RecursiveIterator, SeekableIterator
{
    /**
     * @var int
     * @cvalue SPL_FILE_OBJECT_DROP_NEW_LINE
     */
    public const DROP_NEW_LINE = UNKNOWN;
    /**
     * @var int
     * @cvalue SPL_FILE_OBJECT_READ_AHEAD
     */
    public const READ_AHEAD = UNKNOWN;
    /**
     * @var int
     * @cvalue SPL_FILE_OBJECT_SKIP_EMPTY
     */
    public const SKIP_EMPTY = UNKNOWN;
    /**
     * @var int
     * @cvalue SPL_FILE_OBJECT_READ_CSV
     */
    public const READ_CSV = UNKNOWN;

    /** @param resource|null $context */
    public function __construct(string $filename, string $mode = "r", bool $useIncludePath = false, $context = null) {}

    /** @tentative-return-type */
    public function rewind(): void {}

    /** @tentative-return-type */
    public function eof(): bool {}

    /** @tentative-return-type */
    public function valid(): bool {}

    /** @tentative-return-type */
    public function fgets(): string {}

    /** @tentative-return-type */
    public function fread(int $length): string|false {}

    /** @tentative-return-type */
    public function fgetcsv(string $separator = ",", string $enclosure = "\"", string $escape = "\\"): array|false {}

    /** @tentative-return-type */
    public function fputcsv(array $fields, string $separator = ",", string $enclosure = "\"", string $escape = "\\", string $eol = "\n"): int|false {}

    /** @tentative-return-type */
    public function setCsvControl(string $separator = ",", string $enclosure = "\"", string $escape = "\\"): void {}

    /** @tentative-return-type */
    public function getCsvControl(): array {}

    /**
     * @param int $wouldBlock
     * @tentative-return-type
     */
    public function flock(int $operation, &$wouldBlock = null): bool {}

    /** @tentative-return-type */
    public function fflush(): bool {}

    /** @tentative-return-type */
    public function ftell(): int|false {}

    /** @tentative-return-type */
    public function fseek(int $offset, int $whence = SEEK_SET): int {}

    /** @tentative-return-type */
    public function fgetc(): string|false {}

    /** @tentative-return-type */
    public function fpassthru(): int {}

    /** @tentative-return-type */
    public function fscanf(string $format, mixed &...$vars): array|int|null {}

    /** @tentative-return-type */
    public function fwrite(string $data, int $length = 0): int|false {}

    /** @tentative-return-type */
    public function fstat(): array {}

    /** @tentative-return-type */
    public function ftruncate(int $size): bool {}

    /** @tentative-return-type */
    public function current(): string|array|false {}

    /** @tentative-return-type */
    public function key(): int {}

    /** @tentative-return-type */
    public function next(): void {}

    /** @tentative-return-type */
    public function setFlags(int $flags): void {}

    /** @tentative-return-type */
    public function getFlags(): int {}

    /** @tentative-return-type */
    public function setMaxLineLen(int $maxLength): void {}

    /** @tentative-return-type */
    public function getMaxLineLen(): int {}

    /** @tentative-return-type */
    public function hasChildren(): false {}

    /** @tentative-return-type */
    public function getChildren(): null {}

    /** @tentative-return-type */
    public function seek(int $line): void {}

    /**
     * @tentative-return-type
     * @alias SplFileObject::fgets
     */
    public function getCurrentLine(): string {}

    public function __toString(): string {}
}

class SplTempFileObject extends SplFileObject
{
    public function __construct(int $maxMemory = 2 * 1024 * 1024) {}
}
