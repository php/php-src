--TEST--
Test array_push() function : error conditions - min and max int values as keys
--FILE--
<?php
/* Prototype  : int array_push(array $stack, mixed $var [, mixed $...])
 * Description: Pushes elements onto the end of the array 
 * Source code: ext/standard/array.c
 */

/*
 * Use PHP's minimum and maximum integer values as array keys
 * then try and push new elements onto the array
 */

echo "*** Testing array_push() : error conditions ***\n";

$array = array(-PHP_INT_MAX => 'min', PHP_INT_MAX => 'max');

var_dump(array_push($array, 'new'));
var_dump($array);
var_dump(array_push($array, 'var'));
var_dump($array);

echo "Done";
?>

--EXPECTF--
*** Testing array_push() : error conditions ***
int(3)
array(3) {
  [-%d]=>
  string(3) "min"
  [%d]=>
  string(3) "max"
  [-%d]=>
  string(3) "new"
}

Warning: array_push(): Cannot add element to the array as the next element is already occupied in %s on line %d
bool(false)
array(3) {
  [-%d]=>
  string(3) "min"
  [%d]=>
  string(3) "max"
  [-%d]=>
  string(3) "new"
}
Done
--UEXPECTF--
*** Testing array_push() : error conditions ***
int(3)
array(3) {
  [-2147483647]=>
  unicode(3) "min"
  [2147483647]=>
  unicode(3) "max"
  [-2147483648]=>
  unicode(3) "new"
}

Warning: array_push(): Cannot add element to the array as the next element is already occupied in %s on line %d
bool(false)
array(3) {
  [-2147483647]=>
  unicode(3) "min"
  [2147483647]=>
  unicode(3) "max"
  [-2147483648]=>
  unicode(3) "new"
}
Done