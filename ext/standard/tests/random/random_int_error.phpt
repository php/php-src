--TEST--
Test error operation of random_int()
--FILE--
<?php
//-=-=-=-

var_dump(random_int());

var_dump(random_int(10));

var_dump(random_int(10, 0));

?>
--EXPECTF--
Warning: random_int() expects exactly 2 parameters, 0 given in %s on line %d
NULL

Warning: random_int() expects exactly 2 parameters, 1 given in %s on line %d
NULL

Warning: random_int(): Minimum value must be less than the maximum value in %s on line %d
bool(false)
