--TEST--
Test crc32() function : error conditions
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4)
  die("skip this test is for 32bit platform only");
?>
--FILE--
<?php
/* Prototype  : string crc32(string $str)
 * Description: Calculate the crc32 polynomial of a string
 * Source code: ext/standard/crc32.c
 * Alias to functions: none
*/

/*
 * Testing crc32() : error conditions
*/

echo "*** Testing crc32() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing crc32() function with Zero arguments --\n";
var_dump( crc32() );

//Test crc32 with one more than the expected number of arguments
echo "\n-- Testing crc32() function with more than expected no. of arguments --\n";
$str = 'string_val';
$extra_arg = 10;
var_dump( crc32($str, $extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing crc32() : error conditions ***

-- Testing crc32() function with Zero arguments --

Warning: crc32() expects exactly 1 parameter, 0 given in %s on line %d
NULL

-- Testing crc32() function with more than expected no. of arguments --

Warning: crc32() expects exactly 1 parameter, 2 given in %s on line %d
NULL
Done
