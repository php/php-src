--TEST--
Test mb_internal_encoding() function : basic functionality
--EXTENSIONS--
mbstring
--FILE--
<?php
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

Deprecated: Function mb_internal_encoding() is deprecated since 8.5, use internal_encoding INI settings instead in %s on line %d
string(%d) "%s"

Deprecated: Function mb_internal_encoding() is deprecated since 8.5, use internal_encoding INI settings instead in %s on line %d
bool(true)

Deprecated: Function mb_internal_encoding() is deprecated since 8.5, use internal_encoding INI settings instead in %s on line %d
string(5) "UTF-8"
Done
