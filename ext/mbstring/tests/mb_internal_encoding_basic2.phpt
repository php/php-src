--TEST--
Test mb_internal_encoding() function : basic functionality
--INI--
default_charset=""
input_encoding="ISO-8859-1"
output_encoding="ISO-8859-1"
internal_encoding="ISO-8859-1"
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

var_dump(ini_get('default_charset'));
var_dump(ini_get('input_encoding'));
var_dump(ini_get('output_encoding'));
var_dump(ini_get('internal_encoding'));
var_dump(ini_get('mbstring.http_input'));
var_dump(ini_get('mbstring.http_output'));
var_dump(ini_get('mbstring.internal_encoding'));

var_dump(mb_internal_encoding());   //default internal encoding
var_dump(mb_internal_encoding('UTF-8'));    //change internal encoding to UTF-8
var_dump(mb_internal_encoding());    //check internal encoding is now set to UTF-8

echo "Done";
?>
--EXPECT--
*** Testing mb_internal_encoding() : basic functionality ***
string(0) ""
string(10) "ISO-8859-1"
string(10) "ISO-8859-1"
string(10) "ISO-8859-1"
string(0) ""
string(0) ""
string(0) ""
string(5) "UTF-8"
bool(true)
string(5) "UTF-8"
Done
