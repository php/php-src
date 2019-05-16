--TEST--
Test getrandmax() - basic function test getrandmax()
--FILE--
<?php
$biggest_int = getrandmax();
var_dump($biggest_int);
?>
--EXPECTF--
int(%d)
