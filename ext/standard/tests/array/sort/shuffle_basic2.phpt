--TEST--
Test shuffle() function : basic functionality -  with associative array
--FILE--
<?php
/*
* Test behaviour of shuffle when an associative array is
* passed to the 'array_arg' argument and check for the
* changes in the input array by printing the input array
* before and after shuffle() function is applied on it
*/

echo "*** Testing shuffle() : with associative array ***\n";

// Initialise the associative array
$array_arg = array(
  'one' => 1, 2 => 02, 'three' => 3,
   4 => 4, '#5' => 5, 'SIX' => 6,
  "seven" => 0x7, "#8" => 012, "nine" => 9
);

// printing the input array before the shuffle operation
echo "\n-- input array before shuffle() function is applied --\n";
var_dump( $array_arg );

// applying shuffle() function on the input array
echo "\n-- return value from shuffle() function --\n";
var_dump( shuffle($array_arg) );  // prints the return value from shuffle() function

echo "\n-- resultant array after shuffle() function is applied --\n";
var_dump( $array_arg );

echo "Done";
?>
--EXPECTF--
*** Testing shuffle() : with associative array ***

-- input array before shuffle() function is applied --
array(9) {
  ["one"]=>
  int(1)
  [2]=>
  int(2)
  ["three"]=>
  int(3)
  [4]=>
  int(4)
  ["#5"]=>
  int(5)
  ["SIX"]=>
  int(6)
  ["seven"]=>
  int(7)
  ["#8"]=>
  int(10)
  ["nine"]=>
  int(9)
}

-- return value from shuffle() function --
bool(true)

-- resultant array after shuffle() function is applied --
array(9) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
  [3]=>
  int(%d)
  [4]=>
  int(%d)
  [5]=>
  int(%d)
  [6]=>
  int(%d)
  [7]=>
  int(%d)
  [8]=>
  int(%d)
}
Done
