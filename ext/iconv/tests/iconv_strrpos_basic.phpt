--TEST--
Test iconv_strrpos() function : basic functionality
--SKIPIF--
<?php
extension_loaded('iconv') or die('skip');
function_exists('iconv_strrpos') or die("skip iconv_strrpos() is not available in this build");
?>
--INI--
error_reporting=E_ALL & ~E_DEPRECATED
--FILE--
<?php
/* Prototype  : proto int iconv_strrpos(string haystack, string needle [, string charset])
 * Description: Find position of last occurrence of a string within another 
 * Source code: ext/iconv/iconv.c
 */

/*
 * Test basic functionality of iconv_strrpos()
 */

echo "*** Testing iconv_strrpos() : basic ***\n";

iconv_set_encoding("internal_encoding", "UTF-8");

$string_ascii = b'This is an English string. 0123456789.';
//Japanese string in UTF-8
$string_mb = base64_decode(b'5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII=');

echo "\n-- ASCII string 1 --\n";
var_dump(iconv_strrpos($string_ascii, b'is', 'ISO-8859-1'));

echo "\n-- ASCII string 2 --\n";
var_dump(iconv_strrpos($string_ascii, b'hello, world'));

echo "\n-- Multibyte string 1 --\n";
$needle1 = base64_decode(b'44CC');
var_dump(iconv_strrpos($string_mb, $needle1));

echo "\n-- Multibyte string 2 --\n";
$needle2 = base64_decode(b'44GT44KT44Gr44Gh44Gv44CB5LiW55WM');
var_dump(iconv_strrpos($string_mb, $needle2));

echo "Done";
?>
--EXPECTF--
*** Testing iconv_strrpos() : basic ***

-- ASCII string 1 --
int(15)

-- ASCII string 2 --
bool(false)

-- Multibyte string 1 --
int(20)

-- Multibyte string 2 --
bool(false)
Done

