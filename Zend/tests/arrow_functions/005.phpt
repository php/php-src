--TEST--
Arrow function closing over variable by value
--FILE--
<?php

$b = 2;

var_dump((fn () => ++$b)());
var_dump($b);
?>
--EXPECTF--
int(3)
int(2)

