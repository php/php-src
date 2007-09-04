--TEST--
Test wrong number of arguments for cosh()
--INI--
--FILE--
<?php
/* 
 * proto float cosh(float number)
 * Function is implemented in ext/standard/math.c
*/ 

$arg_0 = 1.0;
$extra_arg = 1;

echo "\nToo many arguments\n";
var_dump(cosh($arg_0, $extra_arg));

echo "\nToo few arguments\n";
var_dump(cosh());

?>
--EXPECTF--
Too many arguments

Warning: Wrong parameter count for cosh() in %s on line 11
NULL

Too few arguments

Warning: Wrong parameter count for cosh() in %s on line 14
NULL
