--TEST--
Test iconv_strpos() function : usage variations - Pass different integers as $offset argument
--EXTENSIONS--
iconv
--INI--
error_reporting=E_ALL & ~E_DEPRECATED
--FILE--
<?php
/*
 * Test how iconv_strpos() behaves when passed different integers as $offset argument
 * The character length of $string_ascii and $string_mb is the same,
 * and the needle appears at the same positions in both strings
 */

iconv_set_encoding("internal_encoding", "UTF-8");

echo "*** Testing iconv_strpos() : usage variations ***\n";

$string_ascii = '+Is an English string'; //21 chars
$needle_ascii = 'g';

$string_mb = base64_decode('5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII='); //21 chars
$needle_mb = base64_decode('44CC');

/*
 * Loop through integers as multiples of ten for $offset argument
 * 60 is larger than *BYTE* count for $string_mb
 */
for ($i = -30; $i <= 60; $i += 10) {
    echo "\n**-- Offset is: $i --**\n";
    echo "-- ASCII String --\n";
    try {
        var_dump(iconv_strpos($string_ascii, $needle_ascii, $i));
    } catch (ValueError $e) {
        echo $e->getMessage(), "\n";
    }
    echo "--Multibyte String --\n";
    try {
        var_dump(iconv_strpos($string_mb, $needle_mb, $i, 'UTF-8'));
    } catch (ValueError $e) {
        echo $e->getMessage(), "\n";
    }
}

echo "Done";
?>
--EXPECT--
*** Testing iconv_strpos() : usage variations ***

**-- Offset is: -30 --**
-- ASCII String --
iconv_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
--Multibyte String --
iconv_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)

**-- Offset is: -20 --**
-- ASCII String --
int(9)
--Multibyte String --
int(9)

**-- Offset is: -10 --**
-- ASCII String --
int(20)
--Multibyte String --
int(20)

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
iconv_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
--Multibyte String --
iconv_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)

**-- Offset is: 40 --**
-- ASCII String --
iconv_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
--Multibyte String --
iconv_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)

**-- Offset is: 50 --**
-- ASCII String --
iconv_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
--Multibyte String --
iconv_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)

**-- Offset is: 60 --**
-- ASCII String --
iconv_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
--Multibyte String --
iconv_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
Done
