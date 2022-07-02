--TEST--
Test mb_strpos() function : mb_strpos bounds check is byte count rather than a character count
--EXTENSIONS--
mbstring
--FILE--
<?php
/*
 * mb_strpos bounds check is byte count rather than a character count:
 * The multibyte string should be returning the same results as the ASCII string.
 * Multibyte string was not returning error message until offset was passed the
 * byte count of the string. Should return error message when passed character count.
 */

$offsets = array(20, 21, 22, 53, 54);
$string_mb = base64_decode('5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII=');
$needle = base64_decode('44CC');

foreach($offsets as $i) {
    echo "\n-- Offset is $i --\n";
    echo "--Multibyte String:--\n";
    try {
        var_dump( mb_strpos($string_mb, $needle, $i, 'UTF-8') );
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    echo"--ASCII String:--\n";
    try {
        var_dump(mb_strpos('This is na English ta', 'a', $i));
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
}
?>
--EXPECT--
-- Offset is 20 --
--Multibyte String:--
int(20)
--ASCII String:--
int(20)

-- Offset is 21 --
--Multibyte String:--
bool(false)
--ASCII String:--
bool(false)

-- Offset is 22 --
--Multibyte String:--
mb_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
--ASCII String:--
mb_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)

-- Offset is 53 --
--Multibyte String:--
mb_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
--ASCII String:--
mb_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)

-- Offset is 54 --
--Multibyte String:--
mb_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
--ASCII String:--
mb_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
