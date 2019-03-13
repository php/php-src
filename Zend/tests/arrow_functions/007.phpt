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
Notice: Undefined variable: a in %s on line %d
NULL

Notice: Undefined variable: 5 in %s on line %d
NULL
