--TEST--
Test  array_splice() function : error conditions
--FILE--
<?php
/* 
 * proto array array_splice(array input, int offset [, int length [, array replacement]])
 * Function is implemented in ext/standard/array.c
*/ 

echo "\n*** Testing error conditions of array_splice() ***\n";

$int=1;
$array=array(1,2);
var_dump (array_splice());
var_dump (array_splice($int));
var_dump (array_splice($array));
var_dump (array_splice($int,$int));
$obj= new stdclass;
var_dump (array_splice($obj,0,1));
echo "Done\n";

?>
--EXPECTF--

*** Testing error conditions of array_splice() ***

Warning: Wrong parameter count for array_splice() in %s on line %d
NULL

Warning: Wrong parameter count for array_splice() in %s on line %d
NULL

Warning: Wrong parameter count for array_splice() in %s on line %d
NULL

Warning: array_splice(): The first argument should be an array in %s on line %d
NULL

Warning: array_splice(): The first argument should be an array in %s on line %d
NULL
Done