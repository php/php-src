--TEST--
Test error operation of random_int()
--FILE--
<?php
//-=-=-=-

var_dump(random_int(10, 0));

?>
--EXPECTF--
Warning: random_int(): Minimum value must be greater than the maximum value in %s on line %d
bool(false)
