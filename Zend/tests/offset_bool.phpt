--TEST--
using different variables to access boolean offsets
--FILE--
<?php

$bool = true;

var_dump($bool[1]);
var_dump($bool[0.0836]);
var_dump($bool[null]);
var_dump($bool["run away"]);

var_dump($bool[true]);
var_dump($bool[false]);

$fp = fopen(__FILE__, "r");
var_dump($bool[$fp]);

$obj = new stdClass();
var_dump($bool[$obj]);

$arr = array(1,2,3);
var_dump($bool[$arr]);

echo "Done\n";

?>
--EXPECTF--
Warning: Trying to access array offset on value of type bool in %s on line %d
NULL

Warning: Trying to access array offset on value of type bool in %s on line %d
NULL

Warning: Trying to access array offset on value of type bool in %s on line %d
NULL

Warning: Trying to access array offset on value of type bool in %s on line %d
NULL

Warning: Trying to access array offset on value of type bool in %s on line %d
NULL

Warning: Trying to access array offset on value of type bool in %s on line %d
NULL

Warning: Trying to access array offset on value of type bool in %s on line %d
NULL

Warning: Trying to access array offset on value of type bool in %s on line %d
NULL

Warning: Trying to access array offset on value of type bool in %s on line %d
NULL
Done
