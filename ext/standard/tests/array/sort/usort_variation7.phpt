--TEST--
Test usort() function : usage variations - Anonymous comparison function
--FILE--
<?php
/*
 * Pass an anonymous comparison function as $cmp_function argument to test behaviour()
 */

echo "*** Testing usort() : usage variation ***\n";

$cmp_function = function($value1, $value2) {
    if ($value1 == $value2) { return 0; }
    else if ($value1 > $value2) { return 1; }
    else { return -1; }
};

$array_arg = array(0 => 100, 1 => 3, 2 => -70, 3 => 24, 4 => 90);

echo "\n-- Anonymous 'cmp_function' with parameters passed by value --\n";
var_dump( usort($array_arg, $cmp_function) );
var_dump($array_arg);

$array_arg = array("b" => "Banana", "m" => "Mango", "a" => "Apple", "p" => "Pineapple");

$cmp_function = function(&$value1, &$value2) {
    if ($value1 == $value2) { return 0; }
    else if ($value1 > $value2) { return 1; }
    else { return -1; }
};

echo "\n-- Anonymous 'cmp_function' with parameters passed by reference --\n";
var_dump( usort($array_arg, $cmp_function) );
var_dump($array_arg);
?>
--EXPECTF--
*** Testing usort() : usage variation ***

-- Anonymous 'cmp_function' with parameters passed by value --
bool(true)
array(5) {
  [0]=>
  int(-70)
  [1]=>
  int(3)
  [2]=>
  int(24)
  [3]=>
  int(90)
  [4]=>
  int(100)
}

-- Anonymous 'cmp_function' with parameters passed by reference --

Warning: {closure:%s:%d}(): Argument #1 ($value1) must be passed by reference, value given in %s on line %d

Warning: {closure:%s:%d}(): Argument #2 ($value2) must be passed by reference, value given in %s on line %d

Warning: {closure:%s:%d}(): Argument #1 ($value1) must be passed by reference, value given in %s on line %d

Warning: {closure:%s:%d}(): Argument #2 ($value2) must be passed by reference, value given in %s on line %d

Warning: {closure:%s:%d}(): Argument #1 ($value1) must be passed by reference, value given in %s on line %d

Warning: {closure:%s:%d}(): Argument #2 ($value2) must be passed by reference, value given in %s on line %d

Warning: {closure:%s:%d}(): Argument #1 ($value1) must be passed by reference, value given in %s on line %d

Warning: {closure:%s:%d}(): Argument #2 ($value2) must be passed by reference, value given in %s on line %d
bool(true)
array(4) {
  [0]=>
  string(5) "Apple"
  [1]=>
  string(6) "Banana"
  [2]=>
  string(5) "Mango"
  [3]=>
  string(9) "Pineapple"
}
