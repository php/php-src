--TEST--
Test mb_strrpos() function : basic functionality
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_strrpos') or die("skip mb_strrpos() is not available in this build");
?>
--FILE--
<?php
/*
 * Test basic functionality of mb_strrpos()
 */

echo "*** Testing mb_strrpos() : basic ***\n";

mb_internal_encoding('UTF-8');

$string_ascii = 'This is an English string. 0123456789.';
//Japanese string in UTF-8
$string_mb = base64_decode('5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII=');

echo "\n-- ASCII string 1 --\n";
var_dump(mb_strrpos($string_ascii, 'is', 4, 'ISO-8859-1'));

echo "\n-- ASCII string 2 --\n";
var_dump(mb_strrpos($string_ascii, 'hello, world'));

echo "\n-- Multibyte string 1 --\n";
$needle1 = base64_decode('44CC');
var_dump(mb_strrpos($string_mb, $needle1));

echo "\n-- Multibyte string 2 --\n";
$needle2 = base64_decode('44GT44KT44Gr44Gh44Gv44CB5LiW55WM');
var_dump(mb_strrpos($string_mb, $needle2));

echo "Done";
?>
--EXPECT--
*** Testing mb_strrpos() : basic ***

-- ASCII string 1 --
int(15)

-- ASCII string 2 --
bool(false)

-- Multibyte string 1 --
int(20)

-- Multibyte string 2 --
bool(false)
Done
