--TEST--
Test usort() function : usage variations - referenced variables
--FILE--
<?php
/* Prototype  : bool usort(array $array_arg, string $cmp_function)
 * Description: Sort an array by values using a user-defined comparison function
 * Source code: ext/standard/array.c
 */

/*
 * Pass an array of referenced variables as $array_arg to test behaviour
 */

echo "*** Testing usort() : usage variation ***\n";

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

// different variables which are used as elements of $array_arg
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

echo "\n-- Sorting \$array_arg containing different references --\n";
var_dump( usort($array_arg, 'cmp_function') );
var_dump($array_arg);
?>
===DONE===
--EXPECTF--
*** Testing usort() : usage variation ***

-- Sorting $array_arg containing different references --
bool(true)
array(5) {
  [0]=>
  &int(-5)
  [1]=>
  &int(0)
  [2]=>
  int(10)
  [3]=>
  &int(100)
  [4]=>
  int(200)
}
===DONE===
