<?php

/** @generate-function-entries */

final class InflateContext
{
}

final class DeflateContext
{
}

function ob_gzhandler(string $data, int $flags): string|false {}

function zlib_get_coding_type(): string|false {}

function gzfile(string $filename, int $use_include_path = 0): array|false {}

/** @return resource|false */
function gzopen(string $filename, string $mode, int $use_include_path = 0) {}

function readgzfile(string $filename, int $use_include_path = 0): int|false {}

function zlib_encode(string $data, int $encoding, int $level = -1): string|false {}

function zlib_decode(string $data, int $max_length = 0): string|false {}

function gzdeflate(string $data, int $level = -1, int $encoding = ZLIB_ENCODING_RAW): string|false {}

function gzencode(string $data, int $level = -1, int $encoding = ZLIB_ENCODING_GZIP): string|false {}

function gzcompress(string $data, int $level = -1, int $encoding = ZLIB_ENCODING_DEFLATE): string|false {}

function gzinflate(string $data, int $max_length = 0): string|false {}

function gzdecode(string $data, int $max_length = 0): string|false {}

function gzuncompress(string $data, int $max_length = 0): string|false {}

/**
 * @param resource $stream
 * @alias fwrite
 */
function gzwrite($stream, string $data, ?int $length = null): int|false {}

/**
 * @param resource $stream
 * @alias fwrite
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

function deflate_init(int $encoding, array $options = []): DeflateContext|false {}

function deflate_add(DeflateContext $context, string $data, int $flush_mode = ZLIB_SYNC_FLUSH): string|false {}

function inflate_init(int $encoding, array $options = []): InflateContext|false {}

function inflate_add(InflateContext $context, string $data, int $flush_mode = ZLIB_SYNC_FLUSH): string|false {}

function inflate_get_status(InflateContext $context): int {}

function inflate_get_read_len(InflateContext $context): int {}
