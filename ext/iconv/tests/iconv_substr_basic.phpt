--TEST--
Test iconv_substr() function : basic functionality
--SKIPIF--
<?php
extension_loaded('iconv') or die('skip');
function_exists('iconv_substr') or die("skip iconv_substr() is not available in this build");
?>
--INI--
iconv.input_encoding=ISO-8859-1
iconv.internal_encoding=ISO-8859-1
iconv.output_encoding=ISO-8859-1
--FILE--
<?php
/* Prototype  : string iconv_substr(string str, int offset, [int length, string charset])
 * Description: Returns part of a string
 * Source code: ext/iconv/iconv.c
 */

/*
 * Test Basic Functionality of iconv_substr with ASCII characters and multibyte strings.
 */

echo "*** Testing iconv_substr() : basic functionality ***\n";

$string_ascii = 'ABCDEF';
//Japanese string in UTF-8
$string_mb = base64_decode('5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII=');

echo "\n-- ASCII string 1 --\n";
var_dump(bin2hex(iconv_substr($string_ascii, 3)));

echo "\n-- ASCII string 2 --\n";
var_dump(bin2hex(iconv_substr($string_ascii, 3, 5, 'ISO-8859-1')));

echo "\n-- Multibyte string 1 --\n";
$result_1 = iconv_substr($string_mb, 2, 7);
var_dump(bin2hex($result_1));

echo "\n-- Multibyte string 2 --\n";
$result_2 = iconv_substr($string_mb, 2, 7, 'utf-8');
var_dump(bin2hex($result_2));

echo "Done";
?>
--EXPECT--
*** Testing iconv_substr() : basic functionality ***

-- ASCII string 1 --
string(6) "444546"

-- ASCII string 2 --
string(6) "444546"

-- Multibyte string 1 --
string(14) "a5e69cace8aa9e"

-- Multibyte string 2 --
string(42) "e8aa9ee38386e382ade382b9e38388e381a7e38199"
Done
