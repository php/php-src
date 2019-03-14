--TEST--
Arrow function closing over variable
--FILE--
<?php

$b = 1;

var_dump((() ==> $b)());

?>
--EXPECT--
int(1)
