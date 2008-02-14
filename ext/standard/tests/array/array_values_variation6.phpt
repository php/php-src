--TEST--
Test array_values() function : usage variations - Referenced variables
--FILE--
<?php
/* Prototype  : array array_values(array $input)
 * Description: Return just the values from the input array 
 * Source code: ext/standard/array.c
 */

/*
 * Test array_values() when:
 * 1. Passed an array made up of referenced variables
 * 2. Passed an array by reference
 */

echo "*** Testing array_values() : usage variations ***\n";

$val1 = 'one';
$val2 = 'two';
$val3 = 'three';

echo "\n-- \$input is an array made up of referenced variables: --\n";
$input = array(&$val1, &$val2, &$val3);
var_dump($result1 = array_values($input));

echo "Change \$val2 and check result of array_values():\n";
$val2 = 'deux';
var_dump($result1);

echo "\n-- Pass \$input argument by reference --\n";
$array = array(1, 2, 3);
var_dump($result2 = array_values(&$array));

echo "Done";
?>

--EXPECTF--
Strict Standards: Call-time pass-by-reference has been deprecated in %s on line %d
*** Testing array_values() : usage variations ***

-- $input is an array made up of referenced variables: --
array(3) {
  [0]=>
  &string(3) "one"
  [1]=>
  &string(3) "two"
  [2]=>
  &string(5) "three"
}
Change $val2 and check result of array_values():
array(3) {
  [0]=>
  &string(3) "one"
  [1]=>
  &string(4) "deux"
  [2]=>
  &string(5) "three"
}

-- Pass $input argument by reference --
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
Done
--UEXPECTF--
Strict Standards: Call-time pass-by-reference has been deprecated in %s on line %d
*** Testing array_values() : usage variations ***

-- $input is an array made up of referenced variables: --
array(3) {
  [0]=>
  &unicode(3) "one"
  [1]=>
  &unicode(3) "two"
  [2]=>
  &unicode(5) "three"
}
Change $val2 and check result of array_values():
array(3) {
  [0]=>
  &unicode(3) "one"
  [1]=>
  &unicode(4) "deux"
  [2]=>
  &unicode(5) "three"
}

-- Pass $input argument by reference --
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
Done