--TEST--
Test mb_ereg_replace() function : basic
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_ereg_replace') or die("skip mb_ereg_replace() is not available in this build");
?>
--FILE--
<?php
/*
 * Test Basic Functionality of mb_ereg_replace()
 */

echo "*** Testing mb_ereg_replace() : basic functionality ***\n";

mb_internal_encoding('UTF-8');
mb_regex_encoding('UTF-8');

$string_ascii = 'abc def';
$string_mb = base64_decode('5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII=');

echo "\n-- ASCII string 1 --\n";
$result_1 = mb_ereg_replace('(.*)def', '\\1 123', $string_ascii);
var_dump(bin2hex($result_1));

echo "\n-- ASCII string 2 --\n";
$result_2 = mb_ereg_replace('123', 'abc', $string_ascii);
var_dump(bin2hex($result_2));

echo "\n-- Multibyte string 1 --\n";
$regex1 = base64_decode('KOaXpeacrOiqnikuKj8oWzEtOV0rKQ==');   //Japanese regex in UTF-8
$result_3 = mb_ereg_replace($regex1, '\\1_____\\2', $string_mb);
var_dump(bin2hex($result_3));

echo "\n-- Multibyte string 2 --\n";
$regex2 = base64_decode('5LiW55WM');
$result_4 = mb_ereg_replace($regex2, '_____', $string_mb);
var_dump(bin2hex($result_4));

echo "Done";
?>
--EXPECT--
*** Testing mb_ereg_replace() : basic functionality ***

-- ASCII string 1 --
string(16) "6162632020313233"

-- ASCII string 2 --
string(14) "61626320646566"

-- Multibyte string 1 --
string(72) "e697a5e69cace8aa9e5f5f5f5f5f31323334efbc95efbc96efbc97efbc98efbc99e38082"

-- Multibyte string 2 --
string(106) "e697a5e69cace8aa9ee38386e382ade382b9e38388e381a7e38199e380823031323334efbc95efbc96efbc97efbc98efbc99e38082"
Done
