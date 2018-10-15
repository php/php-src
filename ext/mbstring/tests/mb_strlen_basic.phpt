--TEST--
Test mb_strlen() function : basic functionality
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_strlen') or die("skip mb_strlen() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : int mb_strlen(string $str [, string $encoding])
 * Description: Get character numbers of a string
 * Source code: ext/mbstring/mbstring.c
 */

/*
 * Test basic functionality of mb_strlen()
 */

echo "*** Testing mb_strlen() : basic functionality***\n";

$string_ascii = 'abc def';
//Japanese string in UTF-8
$string_mb = base64_decode('5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII=');

echo "\n-- ASCII String --\n";
var_dump(mb_strlen($string_ascii));

echo "\n-- Multibyte String --\n";
var_dump(mb_strlen($string_mb, 'UTF-8'));

echo "\nDone";
?>
--EXPECTF--
*** Testing mb_strlen() : basic functionality***

-- ASCII String --
int(7)

-- Multibyte String --
int(21)

Done
