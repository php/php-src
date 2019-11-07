--TEST--
Test gzcompress() function : error conditions
--SKIPIF--
<?php
if (!extension_loaded("zlib")) {
	print "skip - ZLIB extension not loaded";
}
?>
--FILE--
<?php
/* Prototype  : string gzcompress(string data [, int level, [int encoding]])
 * Description: Gzip-compress a string
 * Source code: ext/zlib/zlib.c
 * Alias to functions:
 */

/*
 * add a comment here to say what the test is supposed to do
 */

echo "*** Testing gzcompress() : error conditions ***\n";

echo "\n-- Testing with incorrect compression level --\n";
$data = 'string_val';
$bad_level = 99;
var_dump(gzcompress($data, $bad_level));

echo "\n-- Testing with invalid encoding --\n";
$data = 'string_val';
$level = 2;
$encoding = 99;
var_dump(gzcompress($data, $level, $encoding));

?>
--EXPECTF--
*** Testing gzcompress() : error conditions ***

-- Testing with incorrect compression level --

Warning: gzcompress(): compression level (99) must be within -1..9 in %s on line %d
bool(false)

-- Testing with invalid encoding --

Warning: gzcompress(): encoding mode must be either ZLIB_ENCODING_RAW, ZLIB_ENCODING_GZIP or ZLIB_ENCODING_DEFLATE in %s on line %d
bool(false)
