--TEST--
Test iconv_strpos() function : usage variations - Pass different integers as $offset argument
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
 * Test how iconv_strpos() behaves when passed different integers as $offset argument
 * The character length of $string_ascii and $string_mb is the same, 
 * and the needle appears at the same positions in both strings
 */

iconv_set_encoding("internal_encoding", "UTF-8");

echo "*** Testing iconv_strpos() : usage variations ***\n";

$string_ascii = b'+Is an English string'; //21 chars
$needle_ascii = b'g';

$string_mb = base64_decode(b'5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII='); //21 chars
$needle_mb = base64_decode(b'44CC');

/*
 * Loop through integers as multiples of ten for $offset argument
 * iconv_strpos should not be able to accept negative values as $offset.
 * 60 is larger than *BYTE* count for $string_mb
 */
for ($i = -10; $i <= 60; $i += 10) {
	echo "\n**-- Offset is: $i --**\n";
	echo "-- ASCII String --\n";
	var_dump(iconv_strpos($string_ascii, $needle_ascii, $i));
	echo "--Multibyte String --\n";
	var_dump(iconv_strpos($string_mb, $needle_mb, $i, 'UTF-8'));
}

echo "Done";
?>

--EXPECTF--
*** Testing iconv_strpos() : usage variations ***

**-- Offset is: -10 --**
-- ASCII String --

Warning: iconv_strpos(): Offset not contained in string. in %s on line %d
bool(false)
--Multibyte String --

Warning: iconv_strpos(): Offset not contained in string. in %s on line %d
bool(false)

**-- Offset is: 0 --**
-- ASCII String --
int(9)
--Multibyte String --
int(9)

**-- Offset is: 10 --**
-- ASCII String --
int(20)
--Multibyte String --
int(20)

**-- Offset is: 20 --**
-- ASCII String --
int(20)
--Multibyte String --
int(20)

**-- Offset is: 30 --**
-- ASCII String --
bool(false)
--Multibyte String --
bool(false)

**-- Offset is: 40 --**
-- ASCII String --
bool(false)
--Multibyte String --
bool(false)

**-- Offset is: 50 --**
-- ASCII String --
bool(false)
--Multibyte String --
bool(false)

**-- Offset is: 60 --**
-- ASCII String --
bool(false)
--Multibyte String --
bool(false)
Done