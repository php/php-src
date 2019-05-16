--TEST--
Test mb_strstr() function : variation - multiple needles
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_strstr') or die("skip mb_strstr() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : string mb_strstr(string haystack, string needle[, bool part[, string encoding]])
 * Description: Finds first occurrence of a string within another
 * Source code: ext/mbstring/mbstring.c
 * Alias to functions:
 */

echo "*** Testing mb_strstr() : variation ***\n";

mb_internal_encoding('UTF-8');

//with repeated needles
$string_ascii = 'abcdef zbcdyx';
$needle_ascii = "bcd";

//Japanese string in UTF-8 with repeated needles
$string_mb = base64_decode('5pel5pys6Kqe44OG44Kt44K544OIMzTvvJXvvJbml6XmnKzoqp7jg4bjgq3jgrnjg4g=');
$needle_mb = base64_decode('6Kqe44OG44Kt');

echo "-- Ascii data --\n";
var_dump(bin2hex(mb_strstr($string_ascii, $needle_ascii, false)));
var_dump(bin2hex(mb_strstr($string_ascii, $needle_ascii, true)));

echo "-- mb data in utf-8 --\n";
$res = mb_strstr($string_mb, $needle_mb, false);
if ($res !== false) {
    var_dump(bin2hex($res));
}
else {
   echo "nothing found!\n";
}
$res = mb_strstr($string_mb, $needle_mb, true);
if ($res !== false) {
    var_dump(bin2hex($res));
}
else {
   echo "nothing found!\n";
}


?>
===DONE===
--EXPECT--
*** Testing mb_strstr() : variation ***
-- Ascii data --
string(24) "6263646566207a6263647978"
string(2) "61"
-- mb data in utf-8 --
string(88) "e8aa9ee38386e382ade382b9e383883334efbc95efbc96e697a5e69cace8aa9ee38386e382ade382b9e38388"
string(12) "e697a5e69cac"
===DONE===
