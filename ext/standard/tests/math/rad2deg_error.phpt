--TEST--
Test wrong number of arguments for rad2deg()
--INI--
precision = 14
--FILE--
<?php
/* 
 * proto float rad2deg(float number)
 * Function is implemented in ext/standard/math.c
*/ 

$arg_0 = 1.0;
$extra_arg = 1;

echo "\nToo many arguments\n";
var_dump(rad2deg($arg_0, $extra_arg));

echo "\nToo few arguments\n";
var_dump(rad2deg());

?>
--EXPECTF--
Too many arguments

Warning: Wrong parameter count for rad2deg() in %s on line 11
NULL

Too few arguments

Warning: Wrong parameter count for rad2deg() in %s on line 14
NULL
