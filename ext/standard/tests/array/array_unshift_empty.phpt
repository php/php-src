--TEST--
Test array_unshift() function : prepend array with empty set
--FILE--
<?php
/* Prototype  : int array_unshift(array $array[, mixed ...])
 * Description: Pushes elements onto the beginning of the array
 * Source code: ext/standard/array.c
*/

$array = [1,2,3];
$values = [];

var_dump( array_unshift($array) );
var_dump( array_unshift($array, ...$values) );
var_dump( $array );

echo "Done";
?>
--EXPECT--
int(3)
int(3)
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
Done
