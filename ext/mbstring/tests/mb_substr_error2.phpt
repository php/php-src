--TEST--
Test mb_substr() function : error conditions - Pass an unknown encoding
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_substr') or die("skip mb_substr() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : string mb_substr(string $str, int $start [, int $length [, string $encoding]])
 * Description: Returns part of a string 
 * Source code: ext/mbstring/mbstring.c
 */

/*
 * Pass an unknown encoding to mb_substr() to test behaviour
 */

echo "*** Testing mb_substr() : error conditions ***\n";

$str = 'Hello, world';
$start = 1;
$length = 5;
$encoding = 'unknown-encoding';

var_dump( mb_substr($str, $start, $length, $encoding));

echo "Done";
?>
--EXPECTF--
*** Testing mb_substr() : error conditions ***

Warning: mb_substr(): Unknown encoding "unknown-encoding" in %s on line %d
bool(false)
Done
