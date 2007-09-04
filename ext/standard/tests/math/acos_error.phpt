--TEST--
Test wrong number of arguments for acos()
--INI--
--FILE--
<?php
/* 
 * proto float acos(float number)
 * Function is implemented in ext/standard/math.c
*/ 

$arg_0 = 1.0;
$extra_arg = 1;

echo "\nToo many arguments\n";
var_dump(acos($arg_0, $extra_arg));

echo "\nToo few arguments\n";
var_dump(acos());

?>
--EXPECTF--
Too many arguments

Warning: Wrong parameter count for acos() in %s on line 11
NULL

Too few arguments

Warning: Wrong parameter count for acos() in %s on line 14
NULL
