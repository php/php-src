--TEST--
Test mb_strripos() function : error conditions - Pass unknown encoding
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_strripos') or die("skip mb_strripos() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : int mb_strripos(string haystack, string needle [, int offset [, string encoding]])
 * Description: Finds position of last occurrence of a string within another, case insensitive
 * Source code: ext/mbstring/mbstring.c
 * Alias to functions:
 */

/*
 * Pass an unknown encoding to mb_strripos() to test behaviour
 */

echo "*** Testing mb_strripos() : error conditions ***\n";
$haystack = 'Hello, world';
$needle = 'world';
$offset = 2;
$encoding = 'unknown-encoding';

try {
    var_dump( mb_strripos($haystack, $needle, $offset, $encoding) );
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
*** Testing mb_strripos() : error conditions ***
mb_strripos(): Argument #4 ($encoding) must be a valid encoding, "unknown-encoding" given
