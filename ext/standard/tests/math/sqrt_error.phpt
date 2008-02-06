--TEST--
Test wrong number of arguments for sqrt()
--INI--
precision = 14
--FILE--
<?php
/* 
 * proto float sqrt(float number)
 * Function is implemented in ext/standard/math.c
*/ 

$arg_0 = 1.0;
$extra_arg = 1;

echo "\nToo many arguments\n";
var_dump(sqrt($arg_0, $extra_arg));

echo "\nToo few arguments\n";
var_dump(sqrt());

?>
--EXPECTF--
Too many arguments

Warning: Wrong parameter count for sqrt() in %s on line 11
NULL

Too few arguments

Warning: Wrong parameter count for sqrt() in %s on line 14
NULL
