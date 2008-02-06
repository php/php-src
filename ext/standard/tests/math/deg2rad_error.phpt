--TEST--
Test wrong number of arguments for deg2rad()
--INI--
precision = 14
--FILE--
<?php
/* 
 * proto float deg2rad(float number)
 * Function is implemented in ext/standard/math.c
*/ 

$arg_0 = 1.0;
$extra_arg = 1;

echo "\nToo many arguments\n";
var_dump(deg2rad($arg_0, $extra_arg));

echo "\nToo few arguments\n";
var_dump(deg2rad());

?>
--EXPECTF--
Too many arguments

Warning: Wrong parameter count for deg2rad() in %s on line 11
NULL

Too few arguments

Warning: Wrong parameter count for deg2rad() in %s on line 14
NULL
