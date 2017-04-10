--TEST--
Test soundex() function : basic functionality
--FILE--
<?php
/* Prototype  : string str_rot13  ( string $str  )
 * Description: Perform the rot13 transform on a string
 * Source code: ext/standard/string.c
*/
echo "*** Testing str_rot13() : basic functionality ***\n";

echo "\nBasic tests\n";
var_dump(str_rot13("str_rot13() tests starting"));
var_dump(str_rot13("abcdefghijklmnopqrstuvwxyz"));

echo "\nEnsure numeric characters are left untouched\n";
if (strcmp(str_rot13("0123456789"), "0123456789") == 0) {
	echo "Strings equal : TEST PASSED\n"; 
} else {
	echo "Strings unequal : TEST FAILED\n"; 
}

echo "\nEnsure non-alphabetic characters are left untouched\n";
if (strcmp(str_rot13("!%^&*()_-+={}[]:;@~#<,>.?"), "!%^&*()_-+={}[]:;@~#<,>.?")) {
	echo "Strings equal : TEST PASSED\n"; 
} else {
	echo "Strings unequal : TEST FAILED\n"; 
}

echo "\nEnsure strings round trip\n";
$str = "str_rot13() tests starting";
$encode = str_rot13($str);
$decode = str_rot13($encode);
if (strcmp($str, $decode) == 0) {
	echo "Strings equal : TEST PASSED\n"; 
} else {
	echo "Strings unequal : TEST FAILED\n"; 
}
?>
===DONE===
--EXPECTF--
*** Testing str_rot13() : basic functionality ***

Basic tests
string(26) "fge_ebg13() grfgf fgnegvat"
string(26) "nopqrstuvwxyzabcdefghijklm"

Ensure numeric characters are left untouched
Strings equal : TEST PASSED

Ensure non-alphabetic characters are left untouched
Strings unequal : TEST FAILED

Ensure strings round trip
Strings equal : TEST PASSED
===DONE===