--TEST--
Test array_unshift() function : prepend array with empty set
--FILE--
<?php
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
