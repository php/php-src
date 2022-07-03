--TEST--
Arrow functions implicit use must be throwing notices only upon actual use
--FILE--
<?php

$b = 1;

var_dump((fn() => $b + $c)());

?>
--EXPECTF--
Warning: Undefined variable $c (This will become an error in PHP 9.0) in %s on line %d
int(1)
