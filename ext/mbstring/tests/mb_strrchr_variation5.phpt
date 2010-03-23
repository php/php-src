--TEST--
Test mb_strrchr() function : variation - multiple needles
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

//with repeated needles
$string_ascii = b'abcdef zbcdyx';
$needle_ascii = b"bcd";

//Japanese string in UTF-8 with repeated needles
$string_mb = base64_decode('5pel5pys6Kqe44OG44Kt44K544OIMzTvvJXvvJbml6XmnKzoqp7jg4bjgq3jgrnjg4g=');
$needle_mb = base64_decode('6Kqe44OG44Kt');

echo "-- Ascii data --\n";
var_dump(bin2hex(mb_strrchr($string_ascii, $needle_ascii, false)));
var_dump(bin2hex(mb_strrchr($string_ascii, $needle_ascii, true)));

echo "-- mb data in utf-8 --\n";
$res = mb_strrchr($string_mb, $needle_mb, false);
if ($res !== false) {
    var_dump(bin2hex($res));
}
else {
   echo "nothing found!\n";
}
$res = mb_strrchr($string_mb, $needle_mb, true);
if ($res !== false) {
    var_dump(bin2hex($res));
}
else {
   echo "nothing found!\n";
}


?>
===DONE===
--EXPECT--
*** Testing mb_strrchr() : variation ***
-- Ascii data --
string(10) "6263647978"
string(16) "616263646566207a"
-- mb data in utf-8 --
string(30) "e8aa9ee38386e382ade382b9e38388"
string(70) "e697a5e69cace8aa9ee38386e382ade382b9e383883334efbc95efbc96e697a5e69cac"
===DONE===
