--TEST--
Test mb_substr_count() function : basic functionality
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_substr_count') or die("skip mb_substr_count() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : int mb_substr_count(string $haystack, string $needle [, string $encoding])
 * Description: Count the number of substring occurrences
 * Source code: ext/mbstring/mbstring.c
 */

/*
 * Test Basic functionality of mb_substr_count
 */

echo "*** Testing mb_substr_count() : basic functionality ***\n";

$string_ascii = 'This is an English string. 0123456789.';

$string_mb = base64_decode('5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII=');
$needle_mb1 = base64_decode('44CC');
$needle_mb2 = base64_decode('44GT44KT44Gr44Gh44Gv44CB5LiW55WM');

echo "\n-- ASCII String --\n";
var_dump(mb_substr_count($string_ascii, 'is'));
var_dump(mb_substr_count($string_ascii, 'hello, world'));

echo "\n-- Multibyte String --\n";
var_dump(mb_substr_count($string_mb, $needle_mb1));
var_dump(mb_substr_count($string_mb, $needle_mb2));

echo "Done";
?>
--EXPECTF--
*** Testing mb_substr_count() : basic functionality ***

-- ASCII String --
int(3)
int(0)

-- Multibyte String --
int(2)
int(0)
Done
