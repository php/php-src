--TEST--
Test error operation of random_bytes()
--FILE--
<?php
//-=-=-=-

var_dump(random_bytes());

var_dump(random_bytes(-1));

?>
--EXPECTF--
Warning: random_bytes() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: random_bytes(): Length must be greater than 0 in %s on line %d
bool(false)
