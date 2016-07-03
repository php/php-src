--TEST--
Test iconv_strpos() function : basic functionality
--SKIPIF--
<?php
extension_loaded('iconv') or die('skip');
function_exists('iconv_strpos') or die("skip iconv_strpos() is not available in this build");
?>
--INI--
error_reporting=E_ALL & ~E_DEPRECATED
--FILE--
<?php
/* Prototype  : int iconv_strpos(string haystack, string needle [, int offset [, string charset]])
 * Description: Find position of first occurrence of a string within another 
 * Source code: ext/iconv/iconv.c
 */

/*
 * Test basic functionality of iconv_strpos with ASCII and multibyte characters
 */

echo "*** Testing iconv_strpos() : basic functionality***\n";

iconv_set_encoding("internal_encoding", "UTF-8");

$string_ascii = b'abc def';
//Japanese string in UTF-8
$string_mb = base64_decode(b'5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII=');

echo "\n-- ASCII string 1 --\n";
var_dump(iconv_strpos($string_ascii, b'd', 2, 'ISO-8859-1'));

echo "\n-- ASCII string 2 --\n";
var_dump(iconv_strpos($string_ascii, b'123'));

echo "\n-- Multibyte string 1 --\n";
$needle1 = base64_decode('5pel5pys6Kqe');
var_dump(iconv_strpos($string_mb, $needle1));

echo "\n-- Multibyte string 2 --\n";
$needle2 = base64_decode(b"44GT44KT44Gr44Gh44Gv44CB5LiW55WM");
var_dump(iconv_strpos($string_mb, $needle2));

echo "Done";
?>
--EXPECTF--
*** Testing iconv_strpos() : basic functionality***

-- ASCII string 1 --
int(4)

-- ASCII string 2 --
bool(false)

-- Multibyte string 1 --
int(0)

-- Multibyte string 2 --
bool(false)
Done