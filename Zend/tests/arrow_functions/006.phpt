--TEST--
Nested arrow function closing over variable
--FILE--
<?php

$b = 1;

var_dump((fn () => fn () => $b)()());

?>
--EXPECTF--
int(1)

