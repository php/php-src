--TEST--
array_rand() tests
--FILE--
<?php

var_dump(array_rand());
var_dump(array_rand(array()));
var_dump(array_rand(array(), 0));
var_dump(array_rand(0, 0));
var_dump(array_rand(array(1,2,3), 0));
var_dump(array_rand(array(1,2,3), -1));
var_dump(array_rand(array(1,2,3), 10));
var_dump(array_rand(array(1,2,3), 3));
var_dump(array_rand(array(1,2,3), 2));

echo "Done\n";
?>
--EXPECTF--	
Warning: Wrong parameter count for array_rand() in %s on line %d
NULL
NULL

Warning: array_rand(): Second argument has to be between 1 and the number of elements in the array in %s on line %d
NULL

Warning: array_rand(): First argument has to be an array in %s on line %d
NULL

Warning: array_rand(): Second argument has to be between 1 and the number of elements in the array in %s on line %d
NULL

Warning: array_rand(): Second argument has to be between 1 and the number of elements in the array in %s on line %d
NULL

Warning: array_rand(): Second argument has to be between 1 and the number of elements in the array in %s on line %d
NULL
array(3) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
}
array(2) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
}
Done
