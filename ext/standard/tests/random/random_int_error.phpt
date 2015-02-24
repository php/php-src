--TEST--
Test error operation of random_int()
--FILE--
<?php
//-=-=-=-

var_dump(random_int(10));

var_dump(random_int(10, 0));

?>
--EXPECTF--
Warning: random_int(): A minimum and maximum value are expected, only minimum given in %s on line %d
bool(false)

Warning: random_int(): Minimum value must be less than the maximum value in %s on line %d
bool(false)
