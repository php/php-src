--TEST--
Test mb_internal_encoding() function : basic functionality
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_internal_encoding') or die("skip mb_internal_encoding() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : string mb_internal_encoding([string $encoding])
 * Description: Sets the current internal encoding or Returns
 * the current internal encoding as a string
 * Source code: ext/mbstring/mbstring.c
 */

/*
 * Test basic functionality of mb_internal_encoding
 */

echo "*** Testing mb_internal_encoding() : basic functionality ***\n";

var_dump(mb_internal_encoding());   //default internal encoding

var_dump(mb_internal_encoding('UTF-8'));    //change internal encoding to UTF-8

var_dump(mb_internal_encoding());    //check internal encoding is now set to UTF-8


echo "Done";
?>
--EXPECTF--
*** Testing mb_internal_encoding() : basic functionality ***
string(%d) "%s"
bool(true)
string(5) "UTF-8"
Done
