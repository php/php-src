--TEST--
Test mb_strpos() function : mb_strpos bounds check is byte count rather than a character count
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_strpos') or die("skip mb_strpos() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : int mb_strpos(string $haystack, string $needle [, int $offset [, string $encoding]])
 * Description: Find position of first occurrence of a string within another 
 * Source code: ext/mbstring/mbstring.c
 */

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
	var_dump( mb_strpos($string_mb, $needle, $i, 'UTF-8') );
	echo"--ASCII String:--\n";
	var_dump(mb_strpos(b'This is na English ta', b'a', $i));
}
?>
--EXPECTF--
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

Warning: mb_strpos(): Offset not contained in string. in %s on line %d
bool(false)
--ASCII String:--

Warning: mb_strpos(): Offset not contained in string. in %s on line %d
bool(false)

-- Offset is 53 --
--Multibyte String:--

Warning: mb_strpos(): Offset not contained in string. in %s on line %d
bool(false)
--ASCII String:--

Warning: mb_strpos(): Offset not contained in string. in %s on line %d
bool(false)

-- Offset is 54 --
--Multibyte String:--

Warning: mb_strpos(): Offset not contained in string. in %s on line %d
bool(false)
--ASCII String:--

Warning: mb_strpos(): Offset not contained in string. in %s on line %d
bool(false)