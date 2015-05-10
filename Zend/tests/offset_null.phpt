--TEST--
using different variables to access null offsets
--FILE--
<?php

$null = NULL;

var_dump($null[1]);
var_dump($null[0.0836]);
var_dump($null[NULL]);
var_dump($null["run away"]);

var_dump($null[TRUE]);
var_dump($null[FALSE]);

$fp = fopen(__FILE__, "r");
var_dump($null[$fp]);

$obj = new stdClass;
var_dump($null[$obj]);

$arr = Array(1,2,3);
var_dump($null[$arr]);

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
