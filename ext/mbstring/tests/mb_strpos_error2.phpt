--TEST--
Test mb_strpos() function : error conditions - Pass unknown encoding
--EXTENSIONS--
mbstring
--FILE--
<?php
/*
 * Pass an unknown encoding to mb_strpos() to test behaviour
 */

echo "*** Testing mb_strpos() : error conditions ***\n";
$haystack = 'Hello, world';
$needle = 'world';
$offset = 2;
$encoding = 'unknown-encoding';

try {
    var_dump( mb_strpos($haystack, $needle, $offset, $encoding) );
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
*** Testing mb_strpos() : error conditions ***
mb_strpos(): Argument #4 ($encoding) must be a valid encoding, "unknown-encoding" given
