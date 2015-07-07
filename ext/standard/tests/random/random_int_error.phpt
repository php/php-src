--TEST--
Test error operation of random_int()
--FILE--
<?php
//-=-=-=-

var_dump(random_int());

var_dump(random_int(10));

$randomInt = null;
try {
    $randomInt = random_int(10, 0);
} catch (Exception $e) {
    var_dump($e->getMessage());
}
var_dump($randomInt);

?>
--EXPECTF--
Warning: random_int() expects exactly 2 parameters, 0 given in %s on line %d
NULL

Warning: random_int() expects exactly 2 parameters, 1 given in %s on line %d
NULL
string(61) "Minimum value must be less than or equal to the maximum value"
NULL
