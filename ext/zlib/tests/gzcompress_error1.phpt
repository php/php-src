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
/*
 * add a comment here to say what the test is supposed to do
 */

echo "*** Testing gzcompress() : error conditions ***\n";

echo "\n-- Testing with incorrect compression level --\n";
$data = 'string_val';
$bad_level = 99;
try {
    var_dump(gzcompress($data, $bad_level));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "\n-- Testing with invalid encoding --\n";
$data = 'string_val';
$level = 2;
$encoding = 99;
try {
    var_dump(gzcompress($data, $level, $encoding));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
*** Testing gzcompress() : error conditions ***

-- Testing with incorrect compression level --
gzcompress(): Argument #2 ($level) must be between -1 and 9

-- Testing with invalid encoding --
gzcompress(): Argument #3 ($encoding) must be one of ZLIB_ENCODING_RAW, ZLIB_ENCODING_GZIP, or ZLIB_ENCODING_DEFLATE
