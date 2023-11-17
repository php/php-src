<?php

/** @generate-class-entries */

/**
 * @var int
 * @cvalue PHP_ZLIB_ENCODING_GZIP
 */
const FORCE_GZIP = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_ZLIB_ENCODING_DEFLATE
 */
const FORCE_DEFLATE = UNKNOWN;

/**
 * @var int
 * @cvalue PHP_ZLIB_ENCODING_RAW
 */
const ZLIB_ENCODING_RAW = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_ZLIB_ENCODING_GZIP
 */
const ZLIB_ENCODING_GZIP = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_ZLIB_ENCODING_DEFLATE
 */
const ZLIB_ENCODING_DEFLATE = UNKNOWN;

/**
 * @var int
 * @cvalue Z_NO_FLUSH
 */
const ZLIB_NO_FLUSH = UNKNOWN;
/**
 * @var int
 * @cvalue Z_PARTIAL_FLUSH
 */
const ZLIB_PARTIAL_FLUSH = UNKNOWN;
/**
 * @var int
 * @cvalue Z_SYNC_FLUSH
 */
const ZLIB_SYNC_FLUSH = UNKNOWN;
/**
 * @var int
 * @cvalue Z_FULL_FLUSH
 */
const ZLIB_FULL_FLUSH = UNKNOWN;
/**
 * @var int
 * @cvalue Z_BLOCK
 */
const ZLIB_BLOCK = UNKNOWN;
/**
 * @var int
 * @cvalue Z_FINISH
 */
const ZLIB_FINISH = UNKNOWN;

/**
 * @var int
 * @cvalue Z_FILTERED
 */
const ZLIB_FILTERED = UNKNOWN;
/**
 * @var int
 * @cvalue Z_HUFFMAN_ONLY
 */
const ZLIB_HUFFMAN_ONLY = UNKNOWN;
/**
 * @var int
 * @cvalue Z_RLE
 */
const ZLIB_RLE = UNKNOWN;
/**
 * @var int
 * @cvalue Z_FIXED
 */
const ZLIB_FIXED = UNKNOWN;
/**
 * @var int
 * @cvalue Z_DEFAULT_STRATEGY
 */
const ZLIB_DEFAULT_STRATEGY = UNKNOWN;

/**
 * @var string
 * @cvalue ZLIB_VERSION
 */
const ZLIB_VERSION = UNKNOWN;
/**
 * @var int
 * @cvalue ZLIB_VERNUM
 */
const ZLIB_VERNUM = UNKNOWN;

/**
 * @var int
 * @cvalue Z_OK
 */
const ZLIB_OK = UNKNOWN;
/**
 * @var int
 * @cvalue Z_STREAM_END
 */
const ZLIB_STREAM_END = UNKNOWN;
/**
 * @var int
 * @cvalue Z_NEED_DICT
 */
const ZLIB_NEED_DICT = UNKNOWN;
/**
 * @var int
 * @cvalue Z_ERRNO
 */
const ZLIB_ERRNO = UNKNOWN;
/**
 * @var int
 * @cvalue Z_STREAM_ERROR
 */
const ZLIB_STREAM_ERROR = UNKNOWN;
/**
 * @var int
 * @cvalue Z_DATA_ERROR
 */
const ZLIB_DATA_ERROR = UNKNOWN;
/**
 * @var int
 * @cvalue Z_MEM_ERROR
 */
const ZLIB_MEM_ERROR = UNKNOWN;
/**
 * @var int
 * @cvalue Z_BUF_ERROR
 */
const ZLIB_BUF_ERROR = UNKNOWN;
/**
 * @var int
 * @cvalue Z_VERSION_ERROR
 */
const ZLIB_VERSION_ERROR = UNKNOWN;

/**
 * @strict-properties
 * @not-serializable
 */
final class InflateContext
{
}

/**
 * @strict-properties
 * @not-serializable
 */
final class DeflateContext
{
}

/** @refcount 1 */
function ob_gzhandler(string $data, int $flags): string|false {}

/** @refcount 1 */
function zlib_get_coding_type(): string|false {}

/**
 * @return array<int, string>|false
 * @refcount 1
 */
function gzfile(string $filename, int $use_include_path = 0): array|false {}

/**
 * @return resource|false
 * @refcount 1
 */
function gzopen(string $filename, string $mode, int $use_include_path = 0) {}

function readgzfile(string $filename, int $use_include_path = 0): int|false {}

/** @refcount 1 */
function zlib_encode(string $data, int $encoding, int $level = -1): string|false {}

/** @refcount 1 */
function zlib_decode(string $data, int $max_length = 0): string|false {}

/** @refcount 1 */
function gzdeflate(string $data, int $level = -1, int $encoding = ZLIB_ENCODING_RAW): string|false {}

/** @refcount 1 */
function gzencode(string $data, int $level = -1, int $encoding = ZLIB_ENCODING_GZIP): string|false {}

/** @refcount 1 */
function gzcompress(string $data, int $level = -1, int $encoding = ZLIB_ENCODING_DEFLATE): string|false {}

/** @refcount 1 */
function gzinflate(string $data, int $max_length = 0): string|false {}

/** @refcount 1 */
function gzdecode(string $data, int $max_length = 0): string|false {}

/** @refcount 1 */
function gzuncompress(string $data, int $max_length = 0): string|false {}

/**
 * @param resource $stream
 * @alias fwrite
 */
function gzwrite($stream, string $data, ?int $length = null): int|false {}

/**
 * @param resource $stream
 * @alias fwrite
 * @undocumentable gzputs is an alias of gzwrite, but transitive aliases are not yet supported
 */
function gzputs($stream, string $data, ?int $length = null): int|false {}

/**
 * @param resource $stream
 * @alias rewind
 */
function gzrewind($stream): bool {}

/**
 * @param resource $stream
 * @alias fclose
 */
function gzclose($stream): bool {}

/**
 * @param resource $stream
 * @alias feof
 */
function gzeof($stream): bool {}

/**
 * @param resource $stream
 * @alias fgetc
 */
function gzgetc($stream): string|false {}

/**
 * @param resource $stream
 * @alias fpassthru
 */
function gzpassthru($stream): int {}

/**
 * @param resource $stream
 * @alias fseek
 */
function gzseek($stream, int $offset, int $whence = SEEK_SET): int {}

/**
 * @param resource $stream
 * @alias ftell
 */
function gztell($stream): int|false {}

/**
 * @param resource $stream
 * @alias fread
 */
function gzread($stream, int $length): string|false {}

/**
 * @param resource $stream
 * @alias fgets
 */
function gzgets($stream, ?int $length = null): string|false {}

function deflate_init(int $encoding, array $options = []): DeflateContext {}

function deflate_add(DeflateContext $context, string $data, int $flush_mode = ZLIB_SYNC_FLUSH): string {}

function inflate_init(int $encoding, array $options = []): InflateContext {}

function inflate_add(InflateContext $context, string $data, int $flush_mode = ZLIB_SYNC_FLUSH): string {}

function inflate_get_status(InflateContext $context): int {}

function inflate_get_read_len(InflateContext $context): int {}
