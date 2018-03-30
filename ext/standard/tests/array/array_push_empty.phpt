--TEST--
Test array_push() function : push empty set to the array
--FILE--
<?php
/* Prototype  : int array_push(array $stack[, mixed $...])
 * Description: Pushes elements onto the end of the array
 * Source code: ext/standard/array.c
 */

$array = [1,2,3];
$values = [];

var_dump( array_push($array) );
var_dump( array_push($array, ...$values) );
var_dump( $array );

echo "Done";
?>
--EXPECTF--
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
