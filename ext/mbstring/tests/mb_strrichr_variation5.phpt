--TEST--
Test mb_strrichr() function : usage variation - multiple needles
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_strrichr') or die("skip mb_strrichr() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : string mb_strrichr(string haystack, string needle[, bool part[, string encoding]])
 * Description: Finds the last occurrence of a character in a string within another, case insensitive 
 * Source code: ext/mbstring/mbstring.c
 * Alias to functions: 
 */

echo "*** Testing mb_strrichr() : basic functionality ***\n";

mb_internal_encoding('UTF-8');

//ascii mixed case, multiple needles
$string_ascii = b'abcDef zBcDyx';
$needle_ascii_upper = b"BCD";
$needle_ascii_mixed = b"bCd";
$needle_ascii_lower = b"bcd";

//Greek string in mixed case UTF-8 with multiple needles
$string_mb = base64_decode('zrrOu868zr3Ovs6fzqDOoSDOus67zpzOnc6+zr/OoA==');
$needle_mb_upper = base64_decode('zpzOnc6ezp8=');
$needle_mb_lower = base64_decode('zrzOvc6+zr8=');
$needle_mb_mixed = base64_decode('zpzOnc6+zr8=');

echo "\n-- ASCII string: needle exists --\n";
var_dump(bin2hex(mb_strrichr($string_ascii, $needle_ascii_upper, false)));
var_dump(bin2hex(mb_strrichr($string_ascii, $needle_ascii_upper, true)));
var_dump(bin2hex(mb_strrichr($string_ascii, $needle_ascii_lower, false)));
var_dump(bin2hex(mb_strrichr($string_ascii, $needle_ascii_lower, true)));
var_dump(bin2hex(mb_strrichr($string_ascii, $needle_ascii_mixed, false)));
var_dump(bin2hex(mb_strrichr($string_ascii, $needle_ascii_mixed, true)));


echo "\n-- Multibyte string: needle exists --\n";
var_dump(bin2hex(mb_strrichr($string_mb, $needle_mb_upper, false)));
var_dump(bin2hex(mb_strrichr($string_mb, $needle_mb_upper, true)));
var_dump(bin2hex(mb_strrichr($string_mb, $needle_mb_lower, false)));
var_dump(bin2hex(mb_strrichr($string_mb, $needle_mb_lower, true)));
var_dump(bin2hex(mb_strrichr($string_mb, $needle_mb_mixed, false)));
var_dump(bin2hex(mb_strrichr($string_mb, $needle_mb_mixed, true)));

?>
===DONE===
--EXPECT--
*** Testing mb_strrichr() : basic functionality ***

-- ASCII string: needle exists --
string(10) "4263447978"
string(16) "616263446566207a"
string(10) "4263447978"
string(16) "616263446566207a"
string(10) "4263447978"
string(16) "616263446566207a"

-- Multibyte string: needle exists --
string(20) "ce9cce9dcebecebfcea0"
string(42) "cebacebbcebccebdcebece9fcea0cea120cebacebb"
string(20) "ce9cce9dcebecebfcea0"
string(42) "cebacebbcebccebdcebece9fcea0cea120cebacebb"
string(20) "ce9cce9dcebecebfcea0"
string(42) "cebacebbcebccebdcebece9fcea0cea120cebacebb"
===DONE===