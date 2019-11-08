<?php

function ob_gzhandler(string $data, int $flags): string|false {}

function zlib_get_coding_type(): string|false {}

function gzfile(string $filename, int $use_include_path = 0): array|false {}

/** @return resource|false */
function gzopen(string $filename, string $mode, int $use_include_path = 0) {}

function readgzfile(string $filename, int $use_include_path = 0): int|false {}

function zlib_encode(string $data, int $encoding, int $level = -1): string|false {}

function zlib_decode(string $data, int $max_decoded_len = 0): string|false {}

function gzdeflate(string $data, int $level = -1, int $encoding = ZLIB_ENCODING_RAW): string|false {}

function gzencode(string $data, int $level = -1, int $encoding = ZLIB_ENCODING_GZIP): string|false {}

function gzcompress(string $data, int $level = -1, int $encoding = ZLIB_ENCODING_DEFLATE): string|false {}

function gzinflate(string $data, int $max_decoded_len = 0): string|false {}

function gzdecode(string $data, int $max_decoded_len = 0): string|false {}

function gzuncompress(string $data, int $max_decoded_len = 0): string|false {}

/** @param resource $fp */
function gzwrite($fp, string $str, int $length = UNKNOWN): int|false {};

/** @param resource $fp */
function gzputs($fp, string $str, int $length = UNKNOWN): int|false {};

/** @param resource $fp */
function gzrewind($fp): bool {};

/** @param resource $fp */
function gzclose($fp): bool {};

/** @param resource $fp */
function gzeof($fp): bool {};

/** @param resource $fp */
function gzgetc($fp): string|false {};

/** @param resource $fp */
function gzpassthru($fp): int {};

/** @param resource $fp */
function gzseek($fp, int $offset, int $whence = SEEK_SET): int {};

/** @param resource $fp */
function gztell($fp): int|false {};

/** @param resource $fp */
function gzread($fp, int $length): string|false {};

/** @param resource $fp */
function gzgets($fp, int $length = 1024): string|false {};

/** @return resource|false */
function deflate_init(int $encoding, array $options = []) {};

/** @param resource $resource */
function deflate_add($resource, string $add, int $flush_behavior = ZLIB_SYNC_FLUSH): string|false {};

/** @return resource|false */
function inflate_init(int $encoding, array $options = []) {};

/** @param resource $context */
function inflate_add($context, string $encoded_data, int $flush_mode = ZLIB_SYNC_FLUSH): string|false {};

/** @param resource $resource */
function inflate_get_status($resource): int|false {};

/** @param resource $resource */
function inflate_get_read_len($resource): int|false {};
