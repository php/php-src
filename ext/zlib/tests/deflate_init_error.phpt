--TEST--
Test deflate_init() error
--SKIPIF--
<?php
if (!extension_loaded("zlib")) {
    print "skip - ZLIB extension not loaded";
}
?>
--FILE--
<?php
var_dump(deflate_init(42));
var_dump(deflate_init(ZLIB_ENCODING_DEFLATE, ['level' => 42]));
var_dump(deflate_init(ZLIB_ENCODING_DEFLATE, ['level' => -2]));
var_dump(deflate_init(ZLIB_ENCODING_DEFLATE, ['memory' => 0]));
var_dump(deflate_init(ZLIB_ENCODING_DEFLATE, ['memory' => 10]));
?>
--EXPECTF--
Warning: deflate_init(): encoding mode must be ZLIB_ENCODING_RAW, ZLIB_ENCODING_GZIP or ZLIB_ENCODING_DEFLATE in %s on line %d
bool(false)

Warning: deflate_init(): compression level (42) must be within -1..9 in %s on line %d
bool(false)

Warning: deflate_init(): compression level (-2) must be within -1..9 in %s on line %d
bool(false)

Warning: deflate_init(): compression memory level (0) must be within 1..9 in %s on line %d
bool(false)

Warning: deflate_init(): compression memory level (10) must be within 1..9 in %s on line %d
bool(false)
