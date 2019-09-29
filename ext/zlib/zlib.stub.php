<?php

/** @return string|false */
function ob_gzhandler(string $data, int $flags) {}

/** @return string|false */
function zlib_get_coding_type() {}

/** @return array|false */
function gzfile(string $filename, int $use_include_path = 0) {}

/** @return resource|false */
function gzopen(string $filename, string $mode, int $use_include_path = 0) {}

/** @return int|false */
function readgzfile(string $filename, int $use_include_path = 0) {}

/** @return string|false */
function zlib_encode(string $data, int $encoding, int $level = -1) {}

/** @return string|false */
function zlib_decode(string $data, int $max_decoded_len = 0) {}

/** @return string|false */
function gzdeflate(string $data, int $level = -1, int $encoding = ZLIB_ENCODING_RAW) {}

/** @return string|false */
function gzencode(string $data, int $level = -1, int $encoding = ZLIB_ENCODING_GZIP) {}

/** @return string|false */
function gzcompress(string $data, int $level = -1, int $encoding = ZLIB_ENCODING_DEFLATE) {}

/** @return string|false */
function gzinflate(string $data, int $max_decoded_len = 0) {}

/** @return string|false */
function gzdecode(string $data, int $max_decoded_len = 0) {}

/** @return string|false */
function gzuncompress(string $data, int $max_decoded_len = 0) {}

/**
 * @param resource $fp
 * 
 * @return int|false
 */
function gzwrite($fp, string $str, int $length = UNKNOWN) {};

/**
 * @param resource $fp
 * 
 * @return int|false
 */
function gzputs($fp, string $str, int $length = UNKNOWN) {};

/** @param resource $fp */
function gzrewind($fp): bool {};

/** @param resource $fp */
function gzclose($fp): bool {};

/** @param resource $fp */
function gzeof($fp): bool {};

/**
 * @param resource $fp
 * 
 * @return string|false
 */
function gzgetc($fp) {};

/** @param resource $fp */
function gzpassthru($fp): int {};

/** @param resource $fp */
function gzseek($fp, int $offset, int $whence = SEEK_SET): int {};

/**
 * @param resource $fp
 * 
 * @return int|false
 */
function gztell($fp) {};

/**
 * @param resource $fp
 * 
 * @return string|false
 */
function gzread($fp, int $length) {};

/**
 * @param resource $fp
 * 
 * @return string|false
 */
function gzgets($fp, int $length = 1024) {};

/** @return resource|false */
function deflate_init(int $encoding, array $options = []) {};

/**
 * @param resource $resource
 * 
 * @return string|false
 */
function deflate_add($resource, string $add, int $flush_behavior = ZLIB_SYNC_FLUSH) {};

/** @return resource|false */
function inflate_init(int $encoding, array $options = []) {};

/**
 * @param resource $context
 * 
 * @return string|false
 */
function inflate_add($context, string $encoded_data, int $flush_mode = ZLIB_SYNC_FLUSH) {};

/**
 * @param resource $resource
 * 
 * @return int|false
 */
function inflate_get_status($resource) {};

/**
 * @param resource $resource
 * 
 * @return int|false
 */
function inflate_get_read_len($resource) {};
