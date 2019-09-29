--TEST--
Arrow functions implicit use must be throwing notices only upon actual use
--FILE--
<?php

$b = 1;

var_dump((fn() => $b + $c)());

?>
--EXPECTF--
Notice: Undefined variable: c in %s on line %d
int(1)
