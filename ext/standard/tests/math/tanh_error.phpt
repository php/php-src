--TEST--
Test wrong number of arguments for tanh()
--INI--
--FILE--
<?php
/* 
 * proto float tanh(float number)
 * Function is implemented in ext/standard/math.c
*/ 

$arg_0 = 1.0;
$extra_arg = 1;

echo "\nToo many arguments\n";
var_dump(tanh($arg_0, $extra_arg));

echo "\nToo few arguments\n";
var_dump(tanh());

?>
--EXPECTF--
Too many arguments

Warning: Wrong parameter count for tanh() in %s on line 11
NULL

Too few arguments

Warning: Wrong parameter count for tanh() in %s on line 14
NULL
