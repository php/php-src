--TEST--
Test array_splice() function : error conditions
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

Warning: array_splice() expects at least 2 parameters, 0 given in %s on line %d
NULL

Warning: array_splice() expects at least 2 parameters, 1 given in %s on line %d
NULL

Warning: array_splice() expects at least 2 parameters, 1 given in %s on line %d
NULL

Warning: array_splice() expects parameter 1 to be array, integer given in %s on line %d
NULL

Warning: array_splice() expects parameter 1 to be array, object given in %s on line %d
NULL
Done
