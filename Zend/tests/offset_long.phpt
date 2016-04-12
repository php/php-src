--TEST--
using different variables to access long offsets
--FILE--
<?php

$long = 1;

var_dump($long[1]);
var_dump($long[0.0836]);
var_dump($long[NULL]);
var_dump($long["run away"]);

var_dump($long[TRUE]);
var_dump($long[FALSE]);

$fp = fopen(__FILE__, "r");
var_dump($long[$fp]);

$obj = new stdClass;
var_dump($long[$obj]);

$arr = Array(1,2,3);
var_dump($long[$arr]);

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
