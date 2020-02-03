--TEST--
using different variables to access null offsets
--FILE--
<?php

$null = null;

var_dump($null[1]);
var_dump($null[0.0836]);
var_dump($null[null]);
var_dump($null["run away"]);

var_dump($null[true]);
var_dump($null[false]);

$fp = fopen(__FILE__, "r");
var_dump($null[$fp]);

$obj = new stdClass();
var_dump($null[$obj]);

$arr = array(1,2,3);
var_dump($null[$arr]);

echo "Done\n";

?>
--EXPECTF--
Warning: Trying to access array offset on value of type null in %s on line %d
NULL

Warning: Trying to access array offset on value of type null in %s on line %d
NULL

Warning: Trying to access array offset on value of type null in %s on line %d
NULL

Warning: Trying to access array offset on value of type null in %s on line %d
NULL

Warning: Trying to access array offset on value of type null in %s on line %d
NULL

Warning: Trying to access array offset on value of type null in %s on line %d
NULL

Warning: Trying to access array offset on value of type null in %s on line %d
NULL

Warning: Trying to access array offset on value of type null in %s on line %d
NULL

Warning: Trying to access array offset on value of type null in %s on line %d
NULL
Done
