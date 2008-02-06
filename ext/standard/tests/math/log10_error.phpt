--TEST--
Test wrong number of arguments for log10()
--INI--
precision = 14
--FILE--
<?php
/* 
 * proto float log10(float number)
 * Function is implemented in ext/standard/math.c
*/ 

$arg_0 = 1.0;
$extra_arg = 1;

echo "\nToo many arguments\n";
var_dump(log10($arg_0, $extra_arg));

echo "\nToo few arguments\n";
var_dump(log10());

?>
--EXPECTF--
Too many arguments

Warning: Wrong parameter count for log10() in %s on line 11
NULL

Too few arguments

Warning: Wrong parameter count for log10() in %s on line 14
NULL
