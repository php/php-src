--TEST--
Test md5() function : basic functionality - with raw output
--FILE--
<?php
/* Prototype  : string md5  ( string $str  [, bool $raw_output= false  ] )
 * Description: Calculate the md5 hash of a string
 * Source code: ext/standard/md5.c
*/

echo "*** Testing md5() : basic functionality - with raw output***\n";
$str = b"Hello World";
$md5_raw = md5($str, true);
var_dump(bin2hex($md5_raw));

$md5 = md5($str, false);
	
if (strcmp(bin2hex($md5_raw), $md5) == 0 ) {
	echo "TEST PASSED\n";
} else {
	echo "TEST FAILED\n";
	var_dump($md5_raw, $md5);
}

?>
===DONE===
--EXPECT--
*** Testing md5() : basic functionality - with raw output***
string(32) "b10a8db164e0754105b7a99be72e3fe5"
TEST PASSED
===DONE===