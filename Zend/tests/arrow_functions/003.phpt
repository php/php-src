--TEST--
Variable-variables inside arrow functions
--FILE--
<?php

$a = 1;
$var = "a";
$fn = fn() => $$var;
var_dump($fn());

$b = 2;
$var = "b";
$fn = fn() => { return $$var; };
var_dump($fn());

${5} = 3;
$fn = fn() => ${5};
var_dump($fn());

${6} = 4;
$fn = fn() => { return ${6}; };
var_dump($fn());

?>
--EXPECTF--
Warning: Undefined variable $a in %s on line %d
NULL

Warning: Undefined variable $b in %s on line %d
NULL

Warning: Undefined variable $5 in %s on line %d
NULL

Warning: Undefined variable $6 in %s on line %d
NULL
