--TEST--
Test mb_strtolower() function : error conditions - pass an unknown encoding
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_strtolower') or die("skip mb_strtolower() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : string mb_strtolower(string $sourcestring [, string $encoding])
 * Description: Returns a lowercased version of $sourcestring
 * Source code: ext/mbstring/mbstring.c
 */

/*
 * Pass an unknown encoding to mb_strtolower() to test behaviour
 */

echo "*** Testing mb_strtolower() : error conditions***\n";

$sourcestring = 'hello, world';
$encoding = 'unknown-encoding';

try {
    var_dump( mb_strtolower($sourcestring, $encoding) );
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
*** Testing mb_strtolower() : error conditions***
mb_strtolower(): Argument #2 ($encoding) must be a valid encoding, "unknown-encoding" given
