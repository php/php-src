--TEST--
Closure use inside arrow function
--FILE--
<?php

$b = 1;

var_dump((fn () => function () use ($b) { return $b; })()());

?>
--EXPECTF--
int(1)

