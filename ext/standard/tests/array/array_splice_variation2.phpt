--TEST--
Test array_splice() function : usage variations - additional parameters
--FILE--
<?php
/* 
 * proto array array_splice(array input, int offset [, int length [, array replacement]])
 * Function is implemented in ext/standard/array.c
*/ 

$array=array(0,1,2);
var_dump (array_splice($array,1,1,3,4,5,6,7,8,9));
var_dump ($array);
echo "Done\n";
?>
--EXPECTF--

Warning: Wrong parameter count for array_splice() in %s on line %d
NULL
array(3) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
}
Done