--TEST--
Test function_exists() function : error conditions 
--FILE--
<?php
/* 
 * proto bool function_exists(string function_name)
 * Function is implemented in Zend/zend_builtin_functions.c
*/ 

echo "*** Testing function_exists() : error conditions ***\n";

$arg_0 = "ABC";
$extra_arg = 1;

echo "\nToo many arguments\n";
var_dump(function_exists($arg_0, $extra_arg));

echo "\nToo few arguments\n";
var_dump(function_exists());

?>
===Done===
--EXPECTF--
*** Testing function_exists() : error conditions ***

Too many arguments

Warning: Wrong parameter count for function_exists() in %s on line %d
NULL

Too few arguments

Warning: Wrong parameter count for function_exists() in %s on line %d
NULL
===Done===

