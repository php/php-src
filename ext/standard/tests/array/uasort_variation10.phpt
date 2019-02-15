--TEST--
Test uasort() function : usage variations - sort array with reference variables
--FILE--
<?php
/* Prototype  : bool uasort(array $array_arg, string $cmp_function)
 * Description: Sort an array with a user-defined comparison function and maintain index association
 * Source code: ext/standard/array.c
*/

/*
* Testing uasort() with 'array_arg' containing different reference variables
*/

// comparison function
/* Prototype : int cmp_function(mixed $value1, mixed $value2)
 * Parameters : $value1 and $value2 - values to be compared
 * Return value : 0 - if both values are same
 *                1 - if value1 is greater than value2
 *               -1 - if value1 is less than value2
 * Description : compares value1 and value2
 */
function cmp_function($value1, $value2)
{
  if($value1 == $value2) {
    return 0;
  }
  else if($value1 > $value2) {
    return 1;
  }
  else {
    return -1;
  }
}

echo "*** Testing uasort() : 'array_arg' with elements as reference ***\n";

// different variables which are used as elements of 'array_arg'
$value1 = -5;
$value2 = 100;
$value3 = 0;
$value4 = &$value1;

// array_args an array containing elements with reference variables
$array_arg = array(
  0 => 10,
  1 => &$value4,
  2 => &$value2,
  3 => 200,
  4 => &$value3,
);

echo "-- Sorting 'array_arg' containing different references --\n";
var_dump( uasort($array_arg, 'cmp_function') );  // expecting: bool(true)
var_dump($array_arg);

echo "Done"
?>
--EXPECT--
*** Testing uasort() : 'array_arg' with elements as reference ***
-- Sorting 'array_arg' containing different references --
bool(true)
array(5) {
  [1]=>
  &int(-5)
  [4]=>
  &int(0)
  [0]=>
  int(10)
  [2]=>
  &int(100)
  [3]=>
  int(200)
}
Done
