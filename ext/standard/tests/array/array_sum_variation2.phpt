--TEST--
Test array_sum() function : usage variations - array with different integer value
--FILE--
<?php
/* Prototype  : mixed array_sum(array &input)
 * Description: Returns the sum of the array entries 
 * Source code: ext/standard/array.c
*/

/*
* Testing array_sum() with different types of integer arrays containing data of following type:
*  integer, octal, hexadecimal, maximum and minimum integer values & mixed of all integers
*/

echo "*** Testing array_sum() : different integer array ***\n";

// Int array
$int_values = array(3, 2, 100, 150, 25, 350, 0, -3, -1200);
echo "-- Sum of Integer array --\n";
var_dump( array_sum($int_values) );

// Octal array
$octal_values = array(056, 023, 090, 015, -045, 01, -078);
echo "-- Sum of Octal array --\n";
var_dump( array_sum($octal_values) );

// Hexadecimal array
$hex_values = array(0xAE, 0x2B, 0X10, -0xCF, 0X12, -0XF2);
echo "-- Sum of Hex array --\n";
var_dump( array_sum($hex_values) );

// Mixed values int, octal & hex
$mixed_int_value = array(2, 5, -1, 054, 0X3E, 0, -014, -0x2A);
echo "-- Sum of mixed integer values --\n";
var_dump( array_sum($mixed_int_value) );

echo "Done"
?>
--EXPECTF--
*** Testing array_sum() : different integer array ***
-- Sum of Integer array --
int(-573)
-- Sum of Octal array --
int(35)
-- Sum of Hex array --
int(-198)
-- Sum of mixed integer values --
int(58)
Done
