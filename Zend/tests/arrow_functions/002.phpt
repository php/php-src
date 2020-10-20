--TEST--
Arrow functions implicit use must be throwing notices only upon actual use
--FILE--
<?php

$b = 1;

var_dump((fn() => $b + $c)());

$d = 2;
var_dump((fn() { return $d + $e; } )());

?>
--EXPECTF--
Warning: Undefined variable $c in %s on line %d
int(1)

Warning: Undefined variable $e in %s on line %d
int(2)
