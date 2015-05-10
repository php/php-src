--TEST--
using different variables to access boolean offsets
--FILE--
<?php

$bool = TRUE;

var_dump($bool[1]);
var_dump($bool[0.0836]);
var_dump($bool[NULL]);
var_dump($bool["run away"]);

var_dump($bool[TRUE]);
var_dump($bool[FALSE]);

$fp = fopen(__FILE__, "r");
var_dump($bool[$fp]);

$obj = new stdClass;
var_dump($bool[$obj]);

$arr = Array(1,2,3);
var_dump($bool[$arr]);

echo "Done\n";
?>
--EXPECTF--	

Notice: Trying to get index of a non-array in %s on line 5
NULL

Notice: Trying to get index of a non-array in %s on line 6
NULL

Notice: Trying to get index of a non-array in %s on line 7
NULL

Notice: Trying to get index of a non-array in %s on line 8
NULL

Notice: Trying to get index of a non-array in %s on line 10
NULL

Notice: Trying to get index of a non-array in %s on line 11
NULL

Notice: Trying to get index of a non-array in %s on line 14
NULL

Notice: Trying to get index of a non-array in %s on line 17
NULL

Notice: Trying to get index of a non-array in %s on line 20
NULL
Done
