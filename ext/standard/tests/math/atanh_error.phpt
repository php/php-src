--TEST--
Test wrong number of arguments for atanh()
--SKIPIF--
<?php
if (!function_exists("atanh")) {
	die("SKIP atanh - not supported\n");
}
?> 
--FILE--
<?php
/* 
 * proto float atanh(float number)
 * Function is implemented in ext/standard/math.c
*/ 

$arg_0 = 1.0;
$extra_arg = 1;

echo "\nToo many arguments\n";
var_dump(atanh($arg_0, $extra_arg));

echo "\nToo few arguments\n";
var_dump(atanh());

?>
--EXPECTF--
Too many arguments

Warning: Wrong parameter count for atanh() in %s on line 11
NULL

Too few arguments

Warning: Wrong parameter count for atanh() in %s on line 14
NULL
