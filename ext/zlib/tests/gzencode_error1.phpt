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
gzencode(): Argument #2 ($level) must be between -1 and 9

-- Testing with incorrect encoding_mode --
gzencode(): Argument #3 ($encoding) must be one of ZLIB_ENCODING_RAW, ZLIB_ENCODING_GZIP, or ZLIB_ENCODING_DEFLATE
