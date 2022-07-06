--TEST--
Test mb_strripos() function : usage variations - Pass different integers as $offset argument
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_strripos') or die("skip mb_strripos() is not available in this build");
?>
--FILE--
<?php
/*
 * Test how mb_strripos() behaves when passed different integers as $offset argument
 * The character length of $string_ascii and $string_mb is the same,
 * and the needle appears at the same positions in both strings
 */

mb_internal_encoding('UTF-8');

echo "*** Testing mb_strripos() : usage variations ***\n";

$string_ascii = '+Is an English string'; //21 chars
$needle_ascii = 'G';

$string_mb = base64_decode('5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII='); //21 chars
$needle_mb = base64_decode('44CC');

/*
 * Loop through integers as multiples of ten for $offset argument
 * mb_strripos should not be able to accept negative values as $offset.
 * 60 is larger than *BYTE* count for $string_mb
 */
for ($i = -10; $i <= 60; $i += 10) {
    echo "\n**-- Offset is: $i --**\n";
    echo "-- ASCII String --\n";
    try {
        var_dump(mb_strripos($string_ascii, $needle_ascii, $i));
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }

    echo "--Multibyte String --\n";
    try {
        var_dump(mb_strripos($string_mb, $needle_mb, $i, 'UTF-8'));
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
}

?>
--EXPECT--
*** Testing mb_strripos() : usage variations ***

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
mb_strripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
--Multibyte String --
mb_strripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)

**-- Offset is: 40 --**
-- ASCII String --
mb_strripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
--Multibyte String --
mb_strripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)

**-- Offset is: 50 --**
-- ASCII String --
mb_strripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
--Multibyte String --
mb_strripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)

**-- Offset is: 60 --**
-- ASCII String --
mb_strripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
--Multibyte String --
mb_strripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
