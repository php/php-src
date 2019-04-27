--TEST--
Test mb_internal_encoding() function : error conditions - pass an unknown encoding
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_internal_encoding') or die("skip mb_internal_encoding() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : string mb_internal_encoding([string $encoding])
 * Description: Sets the current internal encoding or
 * Returns the current internal encoding as a string
 * Source code: ext/mbstring/mbstring.c
 */

/*
 * Pass mb_internal_encoding an unknown encoding
 */

echo "*** Testing mb_internal_encoding() : error conditions ***\n";

var_dump(mb_internal_encoding('unknown-encoding'));

echo "Done";
?>
--EXPECTF--
*** Testing mb_internal_encoding() : error conditions ***

Warning: mb_internal_encoding(): Unknown encoding "unknown-encoding" in %s on line %d
bool(false)
Done
