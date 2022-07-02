--TEST--
Test mb_strpos() function : basic functionality
--EXTENSIONS--
mbstring
--FILE--
<?php
/*
 * Test basic functionality of mb_strpos with ASCII and multibyte characters
 */

echo "*** Testing mb_strpos() : basic functionality***\n";

mb_internal_encoding('UTF-8');

$string_ascii = 'abc def';
//Japanese string in UTF-8
$string_mb = base64_decode('5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII=');

echo "\n-- ASCII string 1 --\n";
var_dump(mb_strpos($string_ascii, 'd', 2, 'ISO-8859-1'));

echo "\n-- ASCII string 2 --\n";
var_dump(mb_strpos($string_ascii, '123'));

echo "\n-- Multibyte string 1 --\n";
$needle1 = base64_decode('5pel5pys6Kqe');
var_dump(mb_strpos($string_mb, $needle1));

echo "\n-- Multibyte string 2 --\n";
$needle2 = base64_decode("44GT44KT44Gr44Gh44Gv44CB5LiW55WM");
var_dump(mb_strpos($string_mb, $needle2));

echo "Done";
?>
--EXPECT--
*** Testing mb_strpos() : basic functionality***

-- ASCII string 1 --
int(4)

-- ASCII string 2 --
bool(false)

-- Multibyte string 1 --
int(0)

-- Multibyte string 2 --
bool(false)
Done
