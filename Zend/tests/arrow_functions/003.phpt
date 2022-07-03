--TEST--
Variable-variables inside arrow functions
--FILE--
<?php

$a = 1;
$var = "a";
$fn = fn() => $$var;
var_dump($fn());

${5} = 2;
$fn = fn() => ${5};
var_dump($fn());

?>
--EXPECTF--
Warning: Undefined variable $a (This will become an error in PHP 9.0) in %s on line %d
NULL

Warning: Undefined variable $5 (This will become an error in PHP 9.0) in %s on line %d
NULL
