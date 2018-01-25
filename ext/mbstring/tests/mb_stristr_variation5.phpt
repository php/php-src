--TEST--
Test mb_stristr() function : usage variation - multiple needles
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_stristr') or die("skip mb_stristr() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : string mb_stristr(string haystack, string needle[, bool part[, string encoding]])
 * Description: Finds first occurrence of a string within another, case insensitive
 * Source code: ext/mbstring/mbstring.c
 * Alias to functions:
 */

echo "*** Testing mb_stristr() : basic functionality ***\n";

mb_internal_encoding('UTF-8');

//ascii mixed case, multiple needles
$string_ascii = 'abcDef zBcDyx';
$needle_ascii_upper = "BCD";
$needle_ascii_mixed = "bCd";
$needle_ascii_lower = "bcd";

//Greek string in mixed case UTF-8 with multiple needles
$string_mb = base64_decode('zrrOu868zr3Ovs6fzqDOoSDOus67zpzOnc6+zr/OoA==');
$needle_mb_upper = base64_decode('zpzOnc6ezp8=');
$needle_mb_lower = base64_decode('zrzOvc6+zr8=');
$needle_mb_mixed = base64_decode('zpzOnc6+zr8=');

echo "\n-- ASCII string: needle exists --\n";
var_dump(bin2hex(mb_stristr($string_ascii, $needle_ascii_upper, false)));
var_dump(bin2hex(mb_stristr($string_ascii, $needle_ascii_upper, true)));
var_dump(bin2hex(mb_stristr($string_ascii, $needle_ascii_lower, false)));
var_dump(bin2hex(mb_stristr($string_ascii, $needle_ascii_lower, true)));
var_dump(bin2hex(mb_stristr($string_ascii, $needle_ascii_mixed, false)));
var_dump(bin2hex(mb_stristr($string_ascii, $needle_ascii_mixed, true)));


echo "\n-- Multibyte string: needle exists --\n";
var_dump(bin2hex(mb_stristr($string_mb, $needle_mb_upper, false)));
var_dump(bin2hex(mb_stristr($string_mb, $needle_mb_upper, true)));
var_dump(bin2hex(mb_stristr($string_mb, $needle_mb_lower, false)));
var_dump(bin2hex(mb_stristr($string_mb, $needle_mb_lower, true)));
var_dump(bin2hex(mb_stristr($string_mb, $needle_mb_mixed, false)));
var_dump(bin2hex(mb_stristr($string_mb, $needle_mb_mixed, true)));

?>
===DONE===
--EXPECT--
*** Testing mb_stristr() : basic functionality ***

-- ASCII string: needle exists --
string(24) "6263446566207a4263447978"
string(2) "61"
string(24) "6263446566207a4263447978"
string(2) "61"
string(24) "6263446566207a4263447978"
string(2) "61"

-- Multibyte string: needle exists --
string(54) "cebccebdcebece9fcea0cea120cebacebbce9cce9dcebecebfcea0"
string(8) "cebacebb"
string(54) "cebccebdcebece9fcea0cea120cebacebbce9cce9dcebecebfcea0"
string(8) "cebacebb"
string(54) "cebccebdcebece9fcea0cea120cebacebbce9cce9dcebecebfcea0"
string(8) "cebacebb"
===DONE===
