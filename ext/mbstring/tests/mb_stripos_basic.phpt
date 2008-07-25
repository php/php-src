--TEST--
Test mb_stripos() function : basic functionality
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_stripos') or die("skip mb_stripos() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : int mb_stripos(string $haystack, string $needle [, int $offset [, string $encoding]])
 * Description: Find position of first occurrence of a string within another 
 * Source code: ext/mbstring/mbstring.c
 */

/*
 * Test basic functionality of mb_stripos with ASCII and multibyte characters
 */

echo "*** Testing mb_stripos() : basic functionality***\n";

mb_internal_encoding('UTF-8');

$string_ascii = 'abc def ABC DEF';
//Japanese string in UTF-8
$string_mb = '日本語テキストです。01234５６７８９。';

echo "\n-- ISO-8859-1 string 1 --\n";
var_dump(mb_stripos($string_ascii, 'd', 0, 'ISO-8859-1'));

echo "\n-- ISO-8859-1 string 2 --\n";
var_dump(mb_stripos($string_ascii, 'D', 0, 'ISO-8859-1'));

echo "\n-- ISO-8859-1 string 3 --\n";
var_dump(mb_stripos($string_ascii, 'd', 1, 'ISO-8859-1'));

echo "\n-- ISO-8859-1 string 4 --\n";
var_dump(mb_stripos($string_ascii, 'D', 1, 'ISO-8859-1'));

echo "\n-- ISO-8859-1 string 5 --\n";
var_dump(mb_stripos($string_ascii, 'c', 4, 'ISO-8859-1'));

echo "\n-- ISO-8859-1 string 6 --\n";
var_dump(mb_stripos($string_ascii, 'c D', 0, 'ISO-8859-1'));

echo "\n-- ISO-8859-1 string 7 --\n";
var_dump(mb_stripos($string_ascii, 'C d', 0, 'ISO-8859-1'));

echo "\n-- ISO-8859-1 string 8 --\n";
var_dump(mb_stripos($string_ascii, 'deF', 0, 'ISO-8859-1'));

echo "\n-- ISO-8859-1 string 9 --\n";
var_dump(mb_stripos($string_ascii, '123', 0, 'ISO-8859-1'));

echo "\n-- ISO-8859-1 string 10 --\n";
var_dump(mb_stripos($string_ascii, 'c D', 1, 'ISO-8859-1'));

echo "\n-- ISO-8859-1 string 11 --\n";
var_dump(mb_stripos($string_ascii, 'C d', 1, 'ISO-8859-1'));

echo "\n-- ISO-8859-1 string 12 --\n";
var_dump(mb_stripos($string_ascii, 'deF', 1, 'ISO-8859-1'));

echo "\n-- ISO-8859-1 string 13 --\n";
var_dump(mb_stripos($string_ascii, '123', 1, 'ISO-8859-1'));

echo "\n-- ISO-8859-1 string 14 --\n";
var_dump(mb_stripos($string_ascii, 'c D', 4, 'ISO-8859-1'));

echo "\n-- ISO-8859-1 string 15 --\n";
var_dump(mb_stripos($string_ascii, 'C d', 4, 'ISO-8859-1'));

echo "\n-- ISO-8859-1 string 16 --\n";
var_dump(mb_stripos($string_ascii, 'deF', 4, 'ISO-8859-1'));

echo "\n-- ISO-8859-1 string 17 --\n";
var_dump(mb_stripos($string_ascii, 'deF', 5, 'ISO-8859-1'));

echo "\n-- ISO-8859-1 string 18 --\n";
var_dump(mb_stripos($string_ascii, '123', 4, 'ISO-8859-1'));

echo "\n-- Multibyte string 1 --\n";
$needle1 = '日本語';
var_dump(mb_stripos($string_mb, $needle1));

echo "\n-- Multibyte string 2 --\n";
$needle2 = 'こんにちは、世界';
var_dump(mb_stripos($string_mb, $needle2));

echo "Done";
?>
--EXPECTF--
*** Testing mb_stripos() : basic functionality***

-- ISO-8859-1 string 1 --
int(4)

-- ISO-8859-1 string 2 --
int(4)

-- ISO-8859-1 string 3 --
int(4)

-- ISO-8859-1 string 4 --
int(4)

-- ISO-8859-1 string 5 --
int(10)

-- ISO-8859-1 string 6 --
int(2)

-- ISO-8859-1 string 7 --
int(2)

-- ISO-8859-1 string 8 --
int(4)

-- ISO-8859-1 string 9 --
bool(false)

-- ISO-8859-1 string 10 --
int(2)

-- ISO-8859-1 string 11 --
int(2)

-- ISO-8859-1 string 12 --
int(4)

-- ISO-8859-1 string 13 --
bool(false)

-- ISO-8859-1 string 14 --
int(10)

-- ISO-8859-1 string 15 --
int(10)

-- ISO-8859-1 string 16 --
int(4)

-- ISO-8859-1 string 17 --
int(12)

-- ISO-8859-1 string 18 --
bool(false)

-- Multibyte string 1 --
int(0)

-- Multibyte string 2 --
bool(false)
Done
