--TEST--
Test gzencode() function : error conditions
--SKIPIF--
<?php
if (!extension_loaded("zlib")) {
	print "skip - ZLIB extension not loaded";
}
?>
--FILE--
<?php
/* Prototype  : string gzencode  ( string $data  [, int $level  [, int $encoding_mode  ]] )
 * Description: Gzip-compress a string
 * Source code: ext/zlib/zlib.c
 * Alias to functions:
 */

/*
 * Test error cases for gzencode
 */

echo "*** Testing gzencode() : error conditions ***\n";

$data = 'string_val';
$level = 2;
$encoding_mode = FORCE_DEFLATE;

echo "\n-- Testing with incorrect compression level --\n";
$bad_level = 99;
try {
    var_dump(gzencode($data, $bad_level));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "\n-- Testing with incorrect encoding_mode --\n";
$bad_mode = 99;
try {
    var_dump(gzencode($data, $level, $bad_mode));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
*** Testing gzencode() : error conditions ***

-- Testing with incorrect compression level --
Compression level (99) must be within -1..9

-- Testing with incorrect encoding_mode --
Encoding mode must be either ZLIB_ENCODING_RAW, ZLIB_ENCODING_GZIP or ZLIB_ENCODING_DEFLATE
