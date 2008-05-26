--TEST--
Test array_rand() function : usage variation - invalid values for 'req_num' parameter
--FILE--
<?php
/* Prototype  : mixed array_rand(array $input [, int $num_req])
 * Description: Return key/keys for random entry/entries in the array 
 * Source code: ext/standard/array.c
*/

/*
* Test behaviour of array_rand() function when associative array and 
* various invalid values are passed to the 'input' and 'req_num' 
* parameters respectively
*/

echo "*** Testing array_rand() : with invalid values for 'req_num' ***\n";

// initialise associative arrays
$input = array(
  1 => 'one', 2.2 => 'float key', 0.9 => 'decimal key',
  2e2 => 'exp key1', 2000e-3 => 'negative exp key',
  0xabc => 2748, 0x12f => '303', 0xff => "255",
  0123 => 83, 0129 => 10, 010 => "8"
);
       
// Testing array_rand() function with various invalid 'req_num' values
// with valid num_req values  
echo"\n-- With default num_req value --\n";
var_dump( array_rand($input) );  // with default $num_req value
echo"\n-- With num_req = 1 --\n";
var_dump( array_rand($input, 1) );  // with valid $num_req value

// with invalid num_req value
echo"\n-- With num_req = 0 --\n";
var_dump( array_rand($input, 0) );  // with $num_req=0
echo"\n-- With num_req = -1 --\n";
var_dump( array_rand($input, -1) );  // with $num_req=-1
echo"\n-- With num_req = -2 --\n";
var_dump( array_rand($input, -2) );  // with $num_req=-2
echo"\n-- With num_req more than number of members in 'input' array --\n";
var_dump( array_rand($input, 13) );  // with $num_req=13
 

echo "Done";
?>
--EXPECTF--
*** Testing array_rand() : with invalid values for 'req_num' ***

-- With default num_req value --
int(%d)

-- With num_req = 1 --
int(%d)

-- With num_req = 0 --

Warning: array_rand(): Second argument has to be between 1 and the number of elements in the array in %s on line %d
NULL

-- With num_req = -1 --

Warning: array_rand(): Second argument has to be between 1 and the number of elements in the array in %s on line %d
NULL

-- With num_req = -2 --

Warning: array_rand(): Second argument has to be between 1 and the number of elements in the array in %s on line %d
NULL

-- With num_req more than number of members in 'input' array --

Warning: array_rand(): Second argument has to be between 1 and the number of elements in the array in %s on line %d
NULL
Done
