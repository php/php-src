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
     * @cvalue ZIP_CREATE
     */
    public const int CREATE = UNKNOWN;
    /**
     * @cvalue ZIP_EXCL
     */
    public const int EXCL = UNKNOWN;
    /**
     * @cvalue ZIP_CHECKCONS
     */
    public const int CHECKCONS = UNKNOWN;
    /**
     * @cvalue ZIP_OVERWRITE
     */
    public const int OVERWRITE = UNKNOWN;
#ifdef ZIP_RDONLY
    /**
     * @cvalue ZIP_RDONLY
     */
    public const int RDONLY = UNKNOWN;
#endif

    /**
     * @cvalue ZIP_FL_NOCASE
     */
    public const int FL_NOCASE = UNKNOWN;
    /**
     * @cvalue ZIP_FL_NODIR
     */
    public const int FL_NODIR = UNKNOWN;
    /**
     * @cvalue ZIP_FL_COMPRESSED
     */
    public const int FL_COMPRESSED = UNKNOWN;
    /**
     * @cvalue ZIP_FL_UNCHANGED
     */
    public const int FL_UNCHANGED = UNKNOWN;
/* deprecated in libzip 1.10.0 */
#ifdef ZIP_FL_RECOMPRESS
    /**
     * @cvalue ZIP_FL_RECOMPRESS
     * @deprecated
     */
    public const int FL_RECOMPRESS = UNKNOWN;
#endif
    /**
     * @cvalue ZIP_FL_ENCRYPTED
     */
    public const int FL_ENCRYPTED = UNKNOWN;
    /**
     * @cvalue ZIP_FL_OVERWRITE
     */
    public const int FL_OVERWRITE = UNKNOWN;
    /**
     * @cvalue ZIP_FL_LOCAL
     */
    public const int FL_LOCAL = UNKNOWN;
    /**
     * @cvalue ZIP_FL_CENTRAL
     */
    public const int FL_CENTRAL = UNKNOWN;

    /* Default filename encoding policy. */

    /**
     * @cvalue ZIP_FL_ENC_GUESS
     */
    public const int FL_ENC_GUESS = UNKNOWN;
    /**
     * @cvalue ZIP_FL_ENC_RAW
     */
    public const int FL_ENC_RAW = UNKNOWN;
    /**
     * @cvalue ZIP_FL_ENC_STRICT
     */
    public const int FL_ENC_STRICT = UNKNOWN;
    /**
     * @cvalue ZIP_FL_ENC_UTF_8
     */
    public const int FL_ENC_UTF_8 = UNKNOWN;
    /**
     * @cvalue ZIP_FL_ENC_CP437
     */
    public const int FL_ENC_CP437 = UNKNOWN;

    /**
     * Additionnal flags not from libzip
     * @cvalue ZIP_FL_OPEN_FILE_NOW
     */
    public const int FL_OPEN_FILE_NOW = UNKNOWN;

    /**
     * @cvalue ZIP_CM_DEFAULT
     */
    public const int CM_DEFAULT = UNKNOWN;
    /**
     * @cvalue ZIP_CM_STORE
     */
    public const int CM_STORE = UNKNOWN;
    /**
     * @cvalue ZIP_CM_SHRINK
     */
    public const int CM_SHRINK = UNKNOWN;
    /**
     * @cvalue ZIP_CM_REDUCE_1
     */
    public const int CM_REDUCE_1 = UNKNOWN;
    /**
     * @cvalue ZIP_CM_REDUCE_2
     */
    public const int CM_REDUCE_2 = UNKNOWN;
    /**
     * @cvalue ZIP_CM_REDUCE_3
     */
    public const int CM_REDUCE_3 = UNKNOWN;
    /**
     * @cvalue ZIP_CM_REDUCE_4
     */
    public const int CM_REDUCE_4 = UNKNOWN;
    /**
     * @cvalue ZIP_CM_IMPLODE
     */
    public const int CM_IMPLODE = UNKNOWN;
    /**
     * @cvalue ZIP_CM_DEFLATE
     */
    public const int CM_DEFLATE = UNKNOWN;
    /**
     * @cvalue ZIP_CM_DEFLATE64
     */
    public const int CM_DEFLATE64 = UNKNOWN;
    /**
     * @cvalue ZIP_CM_PKWARE_IMPLODE
     */
    public const int CM_PKWARE_IMPLODE = UNKNOWN;
    /**
     * @cvalue ZIP_CM_BZIP2
     */
    public const int CM_BZIP2 = UNKNOWN;
    /**
     * @cvalue ZIP_CM_LZMA
     */
    public const int CM_LZMA = UNKNOWN;
#ifdef ZIP_CM_LZMA2
    /**
     * @cvalue ZIP_CM_LZMA2
     */
    public const int CM_LZMA2 = UNKNOWN;
#endif

#ifdef ZIP_CM_ZSTD
    /**
     * @cvalue ZIP_CM_ZSTD
     */
    public const int CM_ZSTD = UNKNOWN;
#endif
#ifdef ZIP_CM_XZ
    /**
     * @cvalue ZIP_CM_XZ
     */
    public const int CM_XZ = UNKNOWN;
#endif
    /**
     * @cvalue ZIP_CM_TERSE
     */
    public const int CM_TERSE = UNKNOWN;
    /**
     * @cvalue ZIP_CM_LZ77
     */
    public const int CM_LZ77 = UNKNOWN;
    /**
     * @cvalue ZIP_CM_WAVPACK
     */
    public const int CM_WAVPACK = UNKNOWN;
    /**
     * @cvalue ZIP_CM_PPMD
     */
    public const int CM_PPMD = UNKNOWN;

    /* Error code */

    /**
     * N No error
     * @cvalue ZIP_ER_OK
     */
    public const int ER_OK = UNKNOWN;
    /**
     * N Multi-disk zip archives not supported
     * @cvalue ZIP_ER_MULTIDISK
     */
    public const int ER_MULTIDISK = UNKNOWN;
    /**
     * S Renaming temporary file failed
     * @cvalue ZIP_ER_RENAME
     */
    public const int ER_RENAME = UNKNOWN;
    /**
     * S Closing zip archive failed
     * @cvalue ZIP_ER_CLOSE
     */
    public const int ER_CLOSE = UNKNOWN;
    /**
     * S Seek error
     * @cvalue ZIP_ER_SEEK
     */
    public const int ER_SEEK = UNKNOWN;
    /**
     * S Read error
     * @cvalue ZIP_ER_READ
     */
    public const int ER_READ = UNKNOWN;
    /**
     * S Write error
     * @cvalue ZIP_ER_WRITE
     */
    public const int ER_WRITE = UNKNOWN;
    /**
     * N CRC error
     * @cvalue ZIP_ER_CRC
     */
    public const int ER_CRC = UNKNOWN;
    /**
     * N Containing zip archive was closed
     * @cvalue ZIP_ER_ZIPCLOSED
     */
    public const int ER_ZIPCLOSED = UNKNOWN;
    /**
     * N No such file
     * @cvalue ZIP_ER_NOENT
     */
    public const int ER_NOENT = UNKNOWN;
    /**
     * N File already exists
     * @cvalue ZIP_ER_EXISTS
     */
    public const int ER_EXISTS = UNKNOWN;
    /**
     * S Can't open file
     * @cvalue ZIP_ER_OPEN
     */
    public const int ER_OPEN = UNKNOWN;
    /**
     * S Failure to create temporary file
     * @cvalue ZIP_ER_TMPOPEN
     */
    public const int ER_TMPOPEN = UNKNOWN;
    /**
     * Z Zlib error
     * @cvalue ZIP_ER_ZLIB
     */
    public const int ER_ZLIB = UNKNOWN;
    /**
     * N Malloc failure
     * @cvalue ZIP_ER_MEMORY
     */
    public const int ER_MEMORY = UNKNOWN;
    /**
     * N Entry has been changed
     * @cvalue ZIP_ER_CHANGED
     */
    public const int ER_CHANGED = UNKNOWN;
    /**
     * N Compression method not supported
     * @cvalue ZIP_ER_COMPNOTSUPP
     */
    public const int ER_COMPNOTSUPP = UNKNOWN;
    /**
     * N Premature EOF
     * @cvalue ZIP_ER_EOF
     */
    public const int ER_EOF = UNKNOWN;
    /**
     * N Invalid argument
     * @cvalue ZIP_ER_INVAL
     */
    public const int ER_INVAL = UNKNOWN;
    /**
     * N Not a zip archive
     * @cvalue ZIP_ER_NOZIP
     */
    public const int ER_NOZIP = UNKNOWN;
    /**
     * N Internal error
     * @cvalue ZIP_ER_INTERNAL
     */
    public const int ER_INTERNAL = UNKNOWN;
    /**
     * N Zip archive inconsistent
     * @cvalue ZIP_ER_INCONS
     */
    public const int ER_INCONS = UNKNOWN;
    /**
     * S Can't remove file
     * @cvalue ZIP_ER_REMOVE
     */
    public const int ER_REMOVE = UNKNOWN;
    /**
     * N Entry has been deleted
     * @cvalue ZIP_ER_DELETED
     */
    public const int ER_DELETED = UNKNOWN;
    /**
     * N Encryption method not supported
     * @cvalue ZIP_ER_ENCRNOTSUPP
     */
    public const int ER_ENCRNOTSUPP = UNKNOWN;
    /**
     * N Read-only archive
     * @cvalue ZIP_ER_RDONLY
     */
    public const int ER_RDONLY = UNKNOWN;
    /**
     * N Entry has been deleted
     * @cvalue ZIP_ER_NOPASSWD
     */
    public const int ER_NOPASSWD = UNKNOWN;
    /**
     * N Wrong password provided
     * @cvalue ZIP_ER_WRONGPASSWD
     */
    public const int ER_WRONGPASSWD = UNKNOWN;

    /* since 1.0.0 */
#ifdef ZIP_ER_OPNOTSUPP
    /**
     * N Operation not supported
     * @cvalue ZIP_ER_OPNOTSUPP
     */
    public const int ER_OPNOTSUPP = UNKNOWN;
#endif
#ifdef ZIP_ER_INUSE
    /**
     * N Resource still in use
     * @cvalue ZIP_ER_INUSE
     */
    public const int ER_INUSE = UNKNOWN;
#endif
#ifdef ZIP_ER_TELL
    /**
     * S Tell error
     * @cvalue ZIP_ER_TELL
     */
    public const int ER_TELL = UNKNOWN;
#endif

    /* since 1.6.0 */
#ifdef ZIP_ER_COMPRESSED_DATA
    /**
     * N Compressed data invalid
     * @cvalue ZIP_ER_COMPRESSED_DATA
     */
    public const int ER_COMPRESSED_DATA = UNKNOWN;
#endif
#ifdef ZIP_ER_CANCELLED
    /**
     * N Operation cancelled
     * @cvalue ZIP_ER_CANCELLED
     */
    public const int ER_CANCELLED = UNKNOWN;
#endif

    /* since 1.10.0 */
#ifdef ZIP_ER_DATA_LENGTH
    /**
     * N Unexpected length of data
     * @cvalue ZIP_ER_DATA_LENGTH
     */
    public const int ER_DATA_LENGTH = UNKNOWN;
#endif
#ifdef ZIP_ER_NOT_ALLOWED
    /**
     * Not allowed in torrentzip
     * @cvalue ZIP_ER_NOT_ALLOWED
     */
    public const int ER_NOT_ALLOWED = UNKNOWN;
#endif
#ifdef ZIP_AFL_RDONLY
    /**
     * read only -- cannot be cleared
     * @cvalue ZIP_AFL_RDONLY
     */
    public const int AFL_RDONLY = UNKNOWN;
#endif
#ifdef ZIP_AFL_IS_TORRENTZIP
    /**
     * current archive is torrentzipped
     * @cvalue ZIP_AFL_IS_TORRENTZIP
     */
    public const int AFL_IS_TORRENTZIP = UNKNOWN;
#endif
#ifdef ZIP_AFL_WANT_TORRENTZIP
    /**
     * write archive in torrentzip format
     * @cvalue ZIP_AFL_WANT_TORRENTZIP
     */
    public const int AFL_WANT_TORRENTZIP = UNKNOWN;
#endif
#ifdef ZIP_AFL_CREATE_OR_KEEP_FILE_FOR_EMPTY_ARCHIVE
    /**
     * don't remove file if archive is empty
     * @cvalue ZIP_AFL_CREATE_OR_KEEP_FILE_FOR_EMPTY_ARCHIVE
     */
    public const int AFL_CREATE_OR_KEEP_FILE_FOR_EMPTY_ARCHIVE = UNKNOWN;
#endif

#ifdef ZIP_OPSYS_DEFAULT
    /**
     * @cvalue ZIP_OPSYS_DOS
     * @link ziparchive.constants.opsys
     */
    public const int OPSYS_DOS = UNKNOWN;
    /**
     * @cvalue ZIP_OPSYS_AMIGA
     * @link ziparchive.constants.opsys
     */
    public const int OPSYS_AMIGA = UNKNOWN;
    /**
     * @cvalue ZIP_OPSYS_OPENVMS
     * @link ziparchive.constants.opsys
     */
    public const int OPSYS_OPENVMS = UNKNOWN;
    /**
     * @cvalue ZIP_OPSYS_UNIX
     * @link ziparchive.constants.opsys
     */
    public const int OPSYS_UNIX = UNKNOWN;
    /**
     * @cvalue ZIP_OPSYS_VM_CMS
     * @link ziparchive.constants.opsys
     */
    public const int OPSYS_VM_CMS = UNKNOWN;
    /**
     * @cvalue ZIP_OPSYS_ATARI_ST
     * @link ziparchive.constants.opsys
     */
    public const int OPSYS_ATARI_ST = UNKNOWN;
    /**
     * @cvalue ZIP_OPSYS_OS_2
     * @link ziparchive.constants.opsys
     */
    public const int OPSYS_OS_2 = UNKNOWN;
    /**
     * @cvalue ZIP_OPSYS_MACINTOSH
     * @link ziparchive.constants.opsys
     */
    public const int OPSYS_MACINTOSH = UNKNOWN;
    /**
     * @cvalue ZIP_OPSYS_Z_SYSTEM
     * @link ziparchive.constants.opsys
     */
    public const int OPSYS_Z_SYSTEM = UNKNOWN;
    /**
     * @cvalue ZIP_OPSYS_CPM
     * @link ziparchive.constants.opsys
     */
    public const int OPSYS_CPM = UNKNOWN;
    /**
     * @cvalue ZIP_OPSYS_WINDOWS_NTFS
     * @link ziparchive.constants.opsys
     */
    public const int OPSYS_WINDOWS_NTFS = UNKNOWN;
    /**
     * @cvalue ZIP_OPSYS_MVS
     * @link ziparchive.constants.opsys
     */
    public const int OPSYS_MVS = UNKNOWN;
    /**
     * @cvalue ZIP_OPSYS_VSE
     * @link ziparchive.constants.opsys
     */
    public const int OPSYS_VSE = UNKNOWN;
    /**
     * @cvalue ZIP_OPSYS_ACORN_RISC
     * @link ziparchive.constants.opsys
     */
    public const int OPSYS_ACORN_RISC = UNKNOWN;
    /**
     * @cvalue ZIP_OPSYS_VFAT
     * @link ziparchive.constants.opsys
     */
    public const int OPSYS_VFAT = UNKNOWN;
    /**
     * @cvalue ZIP_OPSYS_ALTERNATE_MVS
     * @link ziparchive.constants.opsys
     */
    public const int OPSYS_ALTERNATE_MVS = UNKNOWN;
    /**
     * @cvalue ZIP_OPSYS_BEOS
     * @link ziparchive.constants.opsys
     */
    public const int OPSYS_BEOS = UNKNOWN;
    /**
     * @cvalue ZIP_OPSYS_TANDEM
     * @link ziparchive.constants.opsys
     */
    public const int OPSYS_TANDEM = UNKNOWN;
    /**
     * @cvalue ZIP_OPSYS_OS_400
     * @link ziparchive.constants.opsys
     */
    public const int OPSYS_OS_400 = UNKNOWN;
    /**
     * @cvalue ZIP_OPSYS_OS_X
     * @link ziparchive.constants.opsys
     */
    public const int OPSYS_OS_X = UNKNOWN;
    /**
     * @cvalue ZIP_OPSYS_DEFAULT
     * @link ziparchive.constants.opsys
     */
    public const int OPSYS_DEFAULT = UNKNOWN;
#endif

    /**
     * @cvalue ZIP_EM_NONE
     */
    public const int EM_NONE = UNKNOWN;
    /**
     * @cvalue ZIP_EM_TRAD_PKWARE
     */
    public const int EM_TRAD_PKWARE = UNKNOWN;
#ifdef HAVE_ENCRYPTION
    /**
     * @cvalue ZIP_EM_AES_128
     */
    public const int EM_AES_128 = UNKNOWN;
    /**
     * @cvalue ZIP_EM_AES_192
     */
    public const int EM_AES_192 = UNKNOWN;
    /**
     * @cvalue ZIP_EM_AES_256
     */
    public const int EM_AES_256 = UNKNOWN;
#endif
    /**
     * @cvalue ZIP_EM_UNKNOWN
     */
    public const int EM_UNKNOWN = UNKNOWN;

    /**
     * @cvalue LIBZIP_VERSION_STR
     */
    public const string LIBZIP_VERSION = UNKNOWN;

    /**
     * @cvalue ZIP_LENGTH_TO_END
     */
    public const int LENGTH_TO_END = UNKNOWN;
    /* since 1.10.1 */
#ifdef ZIP_LENGTH_UNCHECKED
    /**
     * @cvalue ZIP_LENGTH_UNCHECKED
     */
    public const int LENGTH_UNCHECKED = UNKNOWN;
#endif

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
    public function addFile(string $filepath, string $entryname = "", int $start = 0, int $length = ZipArchive::LENGTH_TO_END, int $flags = ZipArchive::FL_OVERWRITE): bool {}

    /** @tentative-return-type */
    public function replaceFile(string $filepath, int $index, int $start = 0, int $length = ZipArchive::LENGTH_TO_END, int $flags = 0): bool {}

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

    public function setArchiveFlag(int $flag, int $value): bool {}

    public function getArchiveFlag(int $flag, int $flags = 0): int {}

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
