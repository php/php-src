--TEST--
Test mb_strrpos() function : usage variations - Pass different integers as $offset arg
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_strrpos') or die("skip mb_strrpos() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : int mb_strrpos(string $haystack, string $needle [, int $offset [, string $encoding]])
 * Description: Find position of last occurrence of a string within another
 * Source code: ext/mbstring/mbstring.c
 */

/*
 * Use different integers in place of $offset to see how mb_strrpos() behaves
 */

echo "*** Testing mb_strrpos() : usage variations ***\n";

$string_ascii = '+Is an English string'; //21 chars
$needle_ascii = 'g';

$string_mb = base64_decode('5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII='); //21 chars
$needle_mb = base64_decode('44CC');

/*
 * Loop through integers as multiples of ten for $offset argument
 * 60 is larger than *BYTE* count for $string_mb
 */
for ($i = -60; $i <= 60; $i += 10) {
	echo "\n**-- Offset is: $i --**\n";
	echo "-- ASCII String --\n";
	var_dump(mb_strrpos($string_ascii, $needle_ascii, $i));
	echo "--Multibyte String --\n";
	var_dump(mb_strrpos($string_mb, $needle_mb, $i, 'UTF-8'));
}

echo "Done";
?>
--EXPECT--
*** Testing mb_strrpos() : usage variations ***

**-- Offset is: -60 --**
-- ASCII String --
bool(false)
--Multibyte String --
bool(false)

**-- Offset is: -50 --**
-- ASCII String --
bool(false)
--Multibyte String --
bool(false)

**-- Offset is: -40 --**
-- ASCII String --
bool(false)
--Multibyte String --
bool(false)

**-- Offset is: -30 --**
-- ASCII String --
bool(false)
--Multibyte String --
bool(false)

**-- Offset is: -20 --**
-- ASCII String --
bool(false)
--Multibyte String --
bool(false)

**-- Offset is: -10 --**
-- ASCII String --
int(9)
--Multibyte String --
int(9)

**-- Offset is: 0 --**
-- ASCII String --
int(20)
--Multibyte String --
int(20)

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
