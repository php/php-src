--TEST--
Test shuffle() function : basic functionality - array with default keys
--FILE--
<?php
/* Prototype  : bool shuffle(array $array_arg)
 * Description: Randomly shuffle the contents of an array 
 * Source code: ext/standard/array.c
*/

/*
* Test behaviour of shuffle when an array with default keys
* is passed to the 'array_arg' argument and check for the
* changes in the input array by printing the input array
* before and after shuffle() function is applied on it
*/

echo "*** Testing shuffle() : with arrays having default keys ***\n";

// Initialise the array with integers
$array_arg_int = array(0, 10, 20, 30, 40, 50, 60, 70, 80);

// Initialise the array with strings
$array_arg_strings = array("one", 'two', 'three', "four", "five", " ", 'six', ' ', "seven");

/* Testing shuffle() function with array of integers */

// printing the input array with integers before the shuffle operation
echo "\n-- input array of integers before shuffle() function is applied --\n";
var_dump( $array_arg_int );

// applying shuffle() function on the input array of integers
echo "\n-- return value from shuffle() function --\n";
var_dump( shuffle($array_arg_int) );  // prints the return value from shuffle() function

echo "\n-- resultant array after shuffle() function is applied --\n";
var_dump( $array_arg_int );

/* Testing shuffle() function with array of strings */

// printing the input array with strings before the shuffle operation
echo "\n-- input array of strings before shuffle() function is applied --\n";
var_dump( $array_arg_strings );

// applying shuffle() function on the input array of strings
echo "\n-- return value from shuffle() function --\n";
var_dump( shuffle($array_arg_strings) );  // prints the return value from shuffle() function

echo "\n-- resultant array after shuffle() function is applied --\n";
var_dump( $array_arg_strings );

echo "Done";
?>
--EXPECTF--
*** Testing shuffle() : with arrays having default keys ***

-- input array of integers before shuffle() function is applied --
array(9) {
  [0]=>
  int(0)
  [1]=>
  int(10)
  [2]=>
  int(20)
  [3]=>
  int(30)
  [4]=>
  int(40)
  [5]=>
  int(50)
  [6]=>
  int(60)
  [7]=>
  int(70)
  [8]=>
  int(80)
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

-- input array of strings before shuffle() function is applied --
array(9) {
  [0]=>
  string(3) "one"
  [1]=>
  string(3) "two"
  [2]=>
  string(5) "three"
  [3]=>
  string(4) "four"
  [4]=>
  string(4) "five"
  [5]=>
  string(1) " "
  [6]=>
  string(3) "six"
  [7]=>
  string(1) " "
  [8]=>
  string(5) "seven"
}

-- return value from shuffle() function --
bool(true)

-- resultant array after shuffle() function is applied --
array(9) {
  [0]=>
  string(%d) "%s"
  [1]=>
  string(%d) "%s"
  [2]=>
  string(%d) "%s"
  [3]=>
  string(%d) "%s"
  [4]=>
  string(%d) "%s"
  [5]=>
  string(%d) "%s"
  [6]=>
  string(%d) "%s"
  [7]=>
  string(%d) "%s"
  [8]=>
  string(%d) "%s"
}
Done
