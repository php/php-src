--TEST--
Test gzdeflate() function : error conditions
--EXTENSIONS--
zlib
--FILE--
<?php
/*
 * add a comment here to say what the test is supposed to do
 */

echo "*** Testing gzdeflate() : error conditions ***\n";

$data = 'string_val';

echo "\n-- Testing with incorrect compression level --\n";
$bad_level = 99;
try {
    var_dump(gzdeflate($data, $bad_level));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "\n-- Testing with incorrect encoding --\n";
$level = 2;
$bad_encoding = 99;
try {
    var_dump(gzdeflate($data, $level, $bad_encoding));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
*** Testing gzdeflate() : error conditions ***

-- Testing with incorrect compression level --
gzdeflate(): Argument #2 ($level) must be between -1 and 9

-- Testing with incorrect encoding --
gzdeflate(): Argument #3 ($encoding) must be one of ZLIB_ENCODING_RAW, ZLIB_ENCODING_GZIP, or ZLIB_ENCODING_DEFLATE
