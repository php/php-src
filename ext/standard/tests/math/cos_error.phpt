--TEST--
Test wrong number of arguments for cos()
--FILE--
<?php
/* 
 * proto float cos(float number)
 * Function is implemented in ext/standard/math.c
*/ 

$arg_0 = 1.0;
$extra_arg = 1;

echo "\nToo many arguments\n";
var_dump(cos($arg_0, $extra_arg));

echo "\nToo few arguments\n";
var_dump(cos());

?>
--EXPECTF--
Too many arguments

Warning: Wrong parameter count for cos() in %s on line 11
NULL

Too few arguments

Warning: Wrong parameter count for cos() in %s on line 14
NULL
