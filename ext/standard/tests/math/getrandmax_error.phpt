--TEST--
Test getrandmax() - wrong params test getrandmax()
--FILE--
<?php
var_dump($biggest_int = getrandmax(true));
?>
--EXPECTF--
Warning: Wrong parameter count for getrandmax() in %s on line 2
NULL
