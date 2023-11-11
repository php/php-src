<?php

/** @generate-class-entries */

/**
 * @return resource|int|false
 * @deprecated
 */
function zip_open(string $filename) {}

/**
 * @param resource $zip
 * @deprecated
 */
function zip_close($zip): void {}

/**
 * @param resource $zip
 * @return resource|false
 * @deprecated
 */
function zip_read($zip) {}

/**
 * @param resource $zip_dp
 * @param resource $zip_entry
 * @deprecated
 */
function zip_entry_open($zip_dp, $zip_entry, string $mode = "rb"): bool {}

/**
 * @param resource $zip_entry
 * @deprecated
 */
function zip_entry_close($zip_entry): bool {}

/**
 * @param resource $zip_entry
 * @deprecated
 */
function zip_entry_read($zip_entry, int $len = 1024): string|false {}

/**
 * @param resource $zip_entry
 * @deprecated
 */
function zip_entry_name($zip_entry): string|false {}

/**
 * @param resource $zip_entry
 * @deprecated
 */
function zip_entry_compressedsize($zip_entry): int|false {}

/**
 * @param resource $zip_entry
 * @deprecated
 */
function zip_entry_filesize($zip_entry): int|false {}

/**
 * @param resource $zip_entry
 * @deprecated
 */
function zip_entry_compressionmethod($zip_entry): string|false {}

class ZipArchive implements Countable
{
    /**
     * @var int
     * @cvalue ZIP_CREATE
     */
    public const CREATE = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_EXCL
     */
    public const EXCL = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_CHECKCONS
     */
    public const CHECKCONS = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_OVERWRITE
     */
    public const OVERWRITE = UNKNOWN;
#ifdef ZIP_RDONLY
    /**
     * @var int
     * @cvalue ZIP_RDONLY
     */
    public const RDONLY = UNKNOWN;
#endif

    /**
     * @var int
     * @cvalue ZIP_FL_NOCASE
     */
    public const FL_NOCASE = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_FL_NODIR
     */
    public const FL_NODIR = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_FL_COMPRESSED
     */
    public const FL_COMPRESSED = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_FL_UNCHANGED
     */
    public const FL_UNCHANGED = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_FL_RECOMPRESS
     */
    public const FL_RECOMPRESS = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_FL_ENCRYPTED
     */
    public const FL_ENCRYPTED = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_FL_OVERWRITE
     */
    public const FL_OVERWRITE = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_FL_LOCAL
     */
    public const FL_LOCAL = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_FL_CENTRAL
     */
    public const FL_CENTRAL = UNKNOWN;

    /* Default filename encoding policy. */

    /**
     * @var int
     * @cvalue ZIP_FL_ENC_GUESS
     */
    public const FL_ENC_GUESS = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_FL_ENC_RAW
     */
    public const FL_ENC_RAW = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_FL_ENC_STRICT
     */
    public const FL_ENC_STRICT = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_FL_ENC_UTF_8
     */
    public const FL_ENC_UTF_8 = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_FL_ENC_CP437
     */
    public const FL_ENC_CP437 = UNKNOWN;

    /**
     * @var int
     * @cvalue ZIP_CM_DEFAULT
     */
    public const CM_DEFAULT = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_CM_STORE
     */
    public const CM_STORE = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_CM_SHRINK
     */
    public const CM_SHRINK = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_CM_REDUCE_1
     */
    public const CM_REDUCE_1 = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_CM_REDUCE_2
     */
    public const CM_REDUCE_2 = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_CM_REDUCE_3
     */
    public const CM_REDUCE_3 = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_CM_REDUCE_4
     */
    public const CM_REDUCE_4 = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_CM_IMPLODE
     */
    public const CM_IMPLODE = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_CM_DEFLATE
     */
    public const CM_DEFLATE = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_CM_DEFLATE64
     */
    public const CM_DEFLATE64 = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_CM_PKWARE_IMPLODE
     */
    public const CM_PKWARE_IMPLODE = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_CM_BZIP2
     */
    public const CM_BZIP2 = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_CM_LZMA
     */
    public const CM_LZMA = UNKNOWN;
#ifdef ZIP_CM_LZMA2
    /**
     * @var int
     * @cvalue ZIP_CM_LZMA2
     */
    public const CM_LZMA2 = UNKNOWN;
#endif

#ifdef ZIP_CM_ZSTD
    /**
     * @var int
     * @cvalue ZIP_CM_ZSTD
     */
    public const CM_ZSTD = UNKNOWN;
#endif
#ifdef ZIP_CM_XZ
    /**
     * @var int
     * @cvalue ZIP_CM_XZ
     */
    public const CM_XZ = UNKNOWN;
#endif
    /**
     * @var int
     * @cvalue ZIP_CM_TERSE
     */
    public const CM_TERSE = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_CM_LZ77
     */
    public const CM_LZ77 = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_CM_WAVPACK
     */
    public const CM_WAVPACK = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_CM_PPMD
     */
    public const CM_PPMD = UNKNOWN;

    /* Error code */

    /**
     * N No error
     * @var int
     * @cvalue ZIP_ER_OK
     */
    public const ER_OK = UNKNOWN;
    /**
     * N Multi-disk zip archives not supported
     * @var int
     * @cvalue ZIP_ER_MULTIDISK
     */
    public const ER_MULTIDISK = UNKNOWN;
    /**
     * S Renaming temporary file failed
     * @var int
     * @cvalue ZIP_ER_RENAME
     */
    public const ER_RENAME = UNKNOWN;
    /**
     * S Closing zip archive failed
     * @var int
     * @cvalue ZIP_ER_CLOSE
     */
    public const ER_CLOSE = UNKNOWN;
    /**
     * S Seek error
     * @var int
     * @cvalue ZIP_ER_SEEK
     */
    public const ER_SEEK = UNKNOWN;
    /**
     * S Read error
     * @var int
     * @cvalue ZIP_ER_READ
     */
    public const ER_READ = UNKNOWN;
    /**
     * S Write error
     * @var int
     * @cvalue ZIP_ER_WRITE
     */
    public const ER_WRITE = UNKNOWN;
    /**
     * N CRC error
     * @var int
     * @cvalue ZIP_ER_CRC
     */
    public const ER_CRC = UNKNOWN;
    /**
     * N Containing zip archive was closed
     * @var int
     * @cvalue ZIP_ER_ZIPCLOSED
     */
    public const ER_ZIPCLOSED = UNKNOWN;
    /**
     * N No such file
     * @var int
     * @cvalue ZIP_ER_NOENT
     */
    public const ER_NOENT = UNKNOWN;
    /**
     * N File already exists
     * @var int
     * @cvalue ZIP_ER_EXISTS
     */
    public const ER_EXISTS = UNKNOWN;
    /**
     * S Can't open file
     * @var int
     * @cvalue ZIP_ER_OPEN
     */
    public const ER_OPEN = UNKNOWN;
    /**
     * S Failure to create temporary file
     * @var int
     * @cvalue ZIP_ER_TMPOPEN
     */
    public const ER_TMPOPEN = UNKNOWN;
    /**
     * Z Zlib error
     * @var int
     * @cvalue ZIP_ER_ZLIB
     */
    public const ER_ZLIB = UNKNOWN;
    /**
     * N Malloc failure
     * @var int
     * @cvalue ZIP_ER_MEMORY
     */
    public const ER_MEMORY = UNKNOWN;
    /**
     * N Entry has been changed
     * @var int
     * @cvalue ZIP_ER_CHANGED
     */
    public const ER_CHANGED = UNKNOWN;
    /**
     * N Compression method not supported
     * @var int
     * @cvalue ZIP_ER_COMPNOTSUPP
     */
    public const ER_COMPNOTSUPP = UNKNOWN;
    /**
     * N Premature EOF
     * @var int
     * @cvalue ZIP_ER_EOF
     */
    public const ER_EOF = UNKNOWN;
    /**
     * N Invalid argument
     * @var int
     * @cvalue ZIP_ER_INVAL
     */
    public const ER_INVAL = UNKNOWN;
    /**
     * N Not a zip archive
     * @var int
     * @cvalue ZIP_ER_NOZIP
     */
    public const ER_NOZIP = UNKNOWN;
    /**
     * N Internal error
     * @var int
     * @cvalue ZIP_ER_INTERNAL
     */
    public const ER_INTERNAL = UNKNOWN;
    /**
     * N Zip archive inconsistent
     * @var int
     * @cvalue ZIP_ER_INCONS
     */
    public const ER_INCONS = UNKNOWN;
    /**
     * S Can't remove file
     * @var int
     * @cvalue ZIP_ER_REMOVE
     */
    public const ER_REMOVE = UNKNOWN;
    /**
     * N Entry has been deleted
     * @var int
     * @cvalue ZIP_ER_DELETED
     */
    public const ER_DELETED = UNKNOWN;
    /**
     * N Encryption method not supported
     * @var int
     * @cvalue ZIP_ER_ENCRNOTSUPP
     */
    public const ER_ENCRNOTSUPP = UNKNOWN;
    /**
     * N Read-only archive
     * @var int
     * @cvalue ZIP_ER_RDONLY
     */
    public const ER_RDONLY = UNKNOWN;
    /**
     * N Entry has been deleted
     * @var int
     * @cvalue ZIP_ER_NOPASSWD
     */
    public const ER_NOPASSWD = UNKNOWN;
    /**
     * N Wrong password provided
     * @var int
     * @cvalue ZIP_ER_WRONGPASSWD
     */
    public const ER_WRONGPASSWD = UNKNOWN;

    /* since 1.0.0 */

#ifdef ZIP_ER_OPNOTSUPP
    /**
     * N Operation not supported
     * @var int
     * @cvalue ZIP_ER_OPNOTSUPP
     */
    public const ER_OPNOTSUPP = UNKNOWN;
#endif
#ifdef ZIP_ER_INUSE
    /**
     * N Resource still in use
     * @var int
     * @cvalue ZIP_ER_INUSE
     */
    public const ER_INUSE = UNKNOWN;
#endif
#ifdef ZIP_ER_TELL
    /**
     * S Tell error
     * @var int
     * @cvalue ZIP_ER_TELL
     */
    public const ER_TELL = UNKNOWN;
#endif

    /* since 1.6.0 */
#ifdef ZIP_ER_COMPRESSED_DATA
    /**
     * N Compressed data invalid
     * @var int
     * @cvalue ZIP_ER_COMPRESSED_DATA
     */
    public const ER_COMPRESSED_DATA = UNKNOWN;
#endif
#ifdef ZIP_ER_CANCELLED
    /**
     * N Operation cancelled
     * @var int
     * @cvalue ZIP_ER_CANCELLED
     */
    public const ER_CANCELLED = UNKNOWN;
#endif

#ifdef ZIP_OPSYS_DEFAULT
    /**
     * @var int
     * @cvalue ZIP_OPSYS_DOS
     * @link ziparchive.constants.opsys
     */
    public const OPSYS_DOS = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_OPSYS_AMIGA
     * @link ziparchive.constants.opsys
     */
    public const OPSYS_AMIGA = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_OPSYS_OPENVMS
     * @link ziparchive.constants.opsys
     */
    public const OPSYS_OPENVMS = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_OPSYS_UNIX
     * @link ziparchive.constants.opsys
     */
    public const OPSYS_UNIX = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_OPSYS_VM_CMS
     * @link ziparchive.constants.opsys
     */
    public const OPSYS_VM_CMS = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_OPSYS_ATARI_ST
     * @link ziparchive.constants.opsys
     */
    public const OPSYS_ATARI_ST = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_OPSYS_OS_2
     * @link ziparchive.constants.opsys
     */
    public const OPSYS_OS_2 = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_OPSYS_MACINTOSH
     * @link ziparchive.constants.opsys
     */
    public const OPSYS_MACINTOSH = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_OPSYS_Z_SYSTEM
     * @link ziparchive.constants.opsys
     */
    public const OPSYS_Z_SYSTEM = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_OPSYS_CPM
     * @link ziparchive.constants.opsys
     */
    public const OPSYS_CPM = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_OPSYS_WINDOWS_NTFS
     * @link ziparchive.constants.opsys
     */
    public const OPSYS_WINDOWS_NTFS = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_OPSYS_MVS
     * @link ziparchive.constants.opsys
     */
    public const OPSYS_MVS = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_OPSYS_VSE
     * @link ziparchive.constants.opsys
     */
    public const OPSYS_VSE = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_OPSYS_ACORN_RISC
     * @link ziparchive.constants.opsys
     */
    public const OPSYS_ACORN_RISC = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_OPSYS_VFAT
     * @link ziparchive.constants.opsys
     */
    public const OPSYS_VFAT = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_OPSYS_ALTERNATE_MVS
     * @link ziparchive.constants.opsys
     */
    public const OPSYS_ALTERNATE_MVS = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_OPSYS_BEOS
     * @link ziparchive.constants.opsys
     */
    public const OPSYS_BEOS = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_OPSYS_TANDEM
     * @link ziparchive.constants.opsys
     */
    public const OPSYS_TANDEM = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_OPSYS_OS_400
     * @link ziparchive.constants.opsys
     */
    public const OPSYS_OS_400 = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_OPSYS_OS_X
     * @link ziparchive.constants.opsys
     */
    public const OPSYS_OS_X = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_OPSYS_DEFAULT
     * @link ziparchive.constants.opsys
     */
    public const OPSYS_DEFAULT = UNKNOWN;
#endif

    /**
     * @var int
     * @cvalue ZIP_EM_NONE
     */
    public const EM_NONE = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_EM_TRAD_PKWARE
     */
    public const EM_TRAD_PKWARE = UNKNOWN;
#ifdef HAVE_ENCRYPTION
    /**
     * @var int
     * @cvalue ZIP_EM_AES_128
     */
    public const EM_AES_128 = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_EM_AES_192
     */
    public const EM_AES_192 = UNKNOWN;
    /**
     * @var int
     * @cvalue ZIP_EM_AES_256
     */
    public const EM_AES_256 = UNKNOWN;
#endif
    /**
     * @var int
     * @cvalue ZIP_EM_UNKNOWN
     */
    public const EM_UNKNOWN = UNKNOWN;

    /**
     * @var string
     * @cvalue LIBZIP_VERSION_STR
     */
    public const LIBZIP_VERSION = UNKNOWN;

    /** @readonly */
    public int $lastId;
    /** @readonly */
    public int $status;
    /** @readonly */
    public int $statusSys;
    /** @readonly */
    public int $numFiles;
    /** @readonly */
    public string $filename;
    /** @readonly */
    public string $comment;

    /** @tentative-return-type */
    public function open(string $filename, int $flags = 0): bool|int {}

    /**
     * @tentative-return-type
     */
    public function setPassword(#[\SensitiveParameter] string $password): bool {}

    /** @tentative-return-type */
    public function close(): bool {}

    /** @tentative-return-type */
    public function count(): int {}

    /** @tentative-return-type */
    public function getStatusString(): string {}

    public function clearError(): void {}

    /** @tentative-return-type */
    public function addEmptyDir(string $dirname, int $flags = 0): bool {}

    /** @tentative-return-type */
    public function addFromString(string $name, string $content, int $flags = ZipArchive::FL_OVERWRITE): bool {}

    /** @tentative-return-type */
    public function addFile(string $filepath, string $entryname = "", int $start = 0, int $length = 0, int $flags = ZipArchive::FL_OVERWRITE): bool {}

    /** @tentative-return-type */
    public function replaceFile(string $filepath, int $index, int $start = 0, int $length = 0, int $flags = 0): bool {}

    /** @tentative-return-type */
    public function addGlob(string $pattern, int $flags = 0, array $options = []): array|false {}

    /** @tentative-return-type */
    public function addPattern(string $pattern, string $path = ".", array $options = []): array|false {}

    /** @tentative-return-type */
    public function renameIndex(int $index, string $new_name): bool {}

    /** @tentative-return-type */
    public function renameName(string $name, string $new_name): bool {}

    /** @tentative-return-type */
    public function setArchiveComment(string $comment): bool {}

    /** @tentative-return-type */
    public function getArchiveComment(int $flags = 0): string|false {}

    /** @tentative-return-type */
    public function setCommentIndex(int $index, string $comment): bool {}

    /** @tentative-return-type */
    public function setCommentName(string $name, string $comment): bool {}

#ifdef HAVE_SET_MTIME
    /** @tentative-return-type */
    public function setMtimeIndex(int $index, int $timestamp, int $flags = 0): bool {}

    /** @tentative-return-type */
    public function setMtimeName(string $name, int $timestamp, int $flags = 0): bool {}
#endif

    /** @tentative-return-type */
    public function getCommentIndex(int $index, int $flags = 0): string|false {}

    /** @tentative-return-type */
    public function getCommentName(string $name, int $flags = 0): string|false {}

    /** @tentative-return-type */
    public function deleteIndex(int $index): bool {}

    /** @tentative-return-type */
    public function deleteName(string $name): bool {}

    /** @tentative-return-type */
    public function statName(string $name, int $flags = 0): array|false {}

    /** @tentative-return-type */
    public function statIndex(int $index, int $flags = 0): array|false {}

    /** @tentative-return-type */
    public function locateName(string $name, int $flags = 0): int|false {}

    /** @tentative-return-type */
    public function getNameIndex(int $index, int $flags = 0): string|false {}

    /** @tentative-return-type */
    public function unchangeArchive(): bool {}

    /** @tentative-return-type */
    public function unchangeAll(): bool {}

    /** @tentative-return-type */
    public function unchangeIndex(int $index): bool {}

    /** @tentative-return-type */
    public function unchangeName(string $name): bool {}

    /** @tentative-return-type */
    public function extractTo(string $pathto, array|string|null $files = null): bool {}

    /** @tentative-return-type */
    public function getFromName(string $name, int $len = 0, int $flags = 0): string|false {}

    /** @tentative-return-type */
    public function getFromIndex(int $index, int $len = 0, int $flags = 0): string|false {}

    /** @return resource|false */
    public function getStreamIndex(int $index, int $flags = 0) {}

    /** @return resource|false */
    public function getStreamName(string $name, int $flags = 0) {}

	/** @return resource|false */
    public function getStream(string $name) {}

#ifdef ZIP_OPSYS_DEFAULT
    /** @tentative-return-type */
    public function setExternalAttributesName(string $name, int $opsys, int $attr, int $flags = 0): bool {}

    /** @tentative-return-type */
    public function setExternalAttributesIndex(int $index, int $opsys, int $attr, int $flags = 0): bool {}

    /**
     * @param int $opsys
     * @param int $attr
     * @tentative-return-type
     */
    public function getExternalAttributesName(string $name, &$opsys, &$attr, int $flags = 0): bool {}

    /**
     * @param int $opsys
     * @param int $attr
     * @tentative-return-type
     */
    public function getExternalAttributesIndex(int $index, &$opsys, &$attr, int $flags = 0): bool {}
#endif

    /** @tentative-return-type */
    public function setCompressionName(string $name, int $method, int $compflags = 0): bool {}

    /** @tentative-return-type */
    public function setCompressionIndex(int $index, int $method, int $compflags = 0): bool {}

#ifdef HAVE_ENCRYPTION
    /**
     * @tentative-return-type
     */
    public function setEncryptionName(string $name, int $method, #[\SensitiveParameter] ?string $password = null): bool {}

    /**
     * @tentative-return-type
     */
    public function setEncryptionIndex(int $index, int $method, #[\SensitiveParameter] ?string $password = null): bool {}
#endif

#ifdef HAVE_PROGRESS_CALLBACK
    /** @tentative-return-type */
    public function registerProgressCallback(float $rate, callable $callback): bool {}
#endif

#ifdef HAVE_CANCEL_CALLBACK
    /** @tentative-return-type */
    public function registerCancelCallback(callable $callback): bool {}
#endif

#ifdef HAVE_METHOD_SUPPORTED
    public static function isCompressionMethodSupported(int $method, bool $enc = true): bool {}

    public static function isEncryptionMethodSupported(int $method, bool $enc = true): bool {}
#endif
}
