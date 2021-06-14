--TEST--
Test mb_strrpos() function : error conditions - pass an unknown encoding
--EXTENSIONS--
mbstring
--FILE--
<?php
/*
 * Pass mb_strrpos() an encoding that doesn't exist
 */

echo "*** Testing mb_strrpos() : error conditions ***\n";

$haystack = 'This is an English string. 0123456789.';
$needle = '123';
$offset = 5;
$encoding = 'unknown-encoding';

try {
    var_dump(mb_strrpos($haystack, $needle , $offset, $encoding));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
*** Testing mb_strrpos() : error conditions ***
mb_strrpos(): Argument #4 ($encoding) must be a valid encoding, "unknown-encoding" given
