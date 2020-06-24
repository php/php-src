--TEST--
Test array_push() function : push empty set to the array
--FILE--
<?php
$array = [1,2,3];
$values = [];

var_dump( array_push($array) );
var_dump( array_push($array, ...$values) );
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
