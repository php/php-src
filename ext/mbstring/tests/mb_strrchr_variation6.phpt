--TEST--
Test mb_strrchr() function : variation - case sensitivity
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_strrchr') or die("skip mb_strrchr() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : string mb_strrchr(string haystack, string needle[, bool part[, string encoding]])
 * Description: Finds the last occurrence of a character in a string within another
 * Source code: ext/mbstring/mbstring.c
 * Alias to functions:
 */

echo "*** Testing mb_strrchr() : variation ***\n";

mb_internal_encoding('UTF-8');

//ascii
$string_ascii = 'abcdef';
$needle_ascii_upper = "BCD";
$needle_ascii_mixed = "bCd";
$needle_ascii_lower = "bcd";

//Greek string in lower case UTF-8
$string_mb = base64_decode('zrHOss6zzrTOtc62zrfOuM65zrrOu868zr3Ovs6/z4DPgc+Dz4TPhc+Gz4fPiM+J');
$needle_mb_upper = base64_decode('zpzOnc6ezp8=');
$needle_mb_lower = base64_decode('zrzOvc6+zr8=');
$needle_mb_mixed = base64_decode('zpzOnc6+zr8=');

echo "-- Ascii data --\n";
// needle should be found
var_dump(bin2hex(mb_strrchr($string_ascii, $needle_ascii_lower)));
// no needle should be found
var_dump(mb_strrchr($string_ascii, $needle_ascii_upper));
var_dump(mb_strrchr($string_ascii, $needle_ascii_mixed));

echo "-- mb data in utf-8 --\n";
// needle should be found
$res = mb_strrchr($string_mb, $needle_mb_lower, false);
if ($res !== false) {
    var_dump(bin2hex($res));
}
else {
   echo "nothing found!\n";
}
// no needle should be found
var_dump(mb_strrchr($string_mb, $needle_mb_upper));
var_dump(mb_strrchr($string_mb, $needle_mb_mixed));


?>
===DONE===
--EXPECT--
*** Testing mb_strrchr() : variation ***
-- Ascii data --
string(10) "6263646566"
bool(false)
bool(false)
-- mb data in utf-8 --
string(52) "cebccebdcebecebfcf80cf81cf83cf84cf85cf86cf87cf88cf89"
bool(false)
bool(false)
===DONE===
