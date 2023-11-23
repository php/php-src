--TEST--
Test mb_internal_encoding() function : basic functionality
--INI--
default_charset=""
input_encoding="ISO-8859-1"
output_encoding="ISO-8859-1"
internal_encoding="ISO-8859-1"
--EXTENSIONS--
mbstring
--FILE--
<?php
/*
 * Test basic functionality of mb_internal_encoding
 */

echo "*** Testing mb_internal_encoding() : basic functionality ***\n";

var_dump(ini_get('default_charset'));
var_dump(ini_get('input_encoding'));
var_dump(ini_get('output_encoding'));
var_dump(ini_get('internal_encoding'));

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
string(10) "ISO-8859-1"
bool(true)
string(5) "UTF-8"
Done
