--TEST--
Test mb_strtoupper() function : error conditions - pass an unknown encoding
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_strtoupper') or die("skip mb_strtoupper() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : string mb_strtoupper(string $sourcestring [, string $encoding]
 * Description: Returns a uppercased version of $sourcestring
 * Source code: ext/mbstring/mbstring.c
 */

/*
 * Pass an unknown encoding as $encoding argument to check behaviour of mbstrtoupper()
 */

echo "*** Testing mb_strtoupper() : error conditions ***\n";

$sourcestring = 'hello, world';
$encoding = 'unknown-encoding';

var_dump( mb_strtoupper($sourcestring, $encoding) );

echo "Done";
?>
--EXPECTF--
*** Testing mb_strtoupper() : error conditions ***

Warning: mb_strtoupper(): Unknown encoding "unknown-encoding" in %s on line %d
bool(false)
Done
