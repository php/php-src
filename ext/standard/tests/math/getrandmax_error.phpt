--TEST--
Test getrandmax() - wrong params test getrandmax()
--FILE--
<?php
var_dump($biggest_int = getrandmax(true));
?>
--EXPECTF--
Warning: getrandmax() expects exactly 0 parameters, 1 given in %s on line 2
NULL
