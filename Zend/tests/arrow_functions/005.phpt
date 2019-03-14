--TEST--
Arrow function closing over variable by value
--FILE--
<?php

$b = 2;

var_dump((() ==> ++$b)());
var_dump($b);
?>
--EXPECT--
int(3)
int(2)
