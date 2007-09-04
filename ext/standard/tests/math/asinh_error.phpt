--TEST--
Test wrong number of arguments for asinh()
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == "WIN" )
        die ("skip - function not supported on Windows");
?>
--INI--
--FILE--
<?php
/* 
 * proto float asinh(float number)
 * Function is implemented in ext/standard/math.c
*/ 

$arg_0 = 1.0;
$extra_arg = 1;

echo "\nToo many arguments\n";
var_dump(asinh($arg_0, $extra_arg));

echo "\nToo few arguments\n";
var_dump(asinh());

?>
--EXPECTF--
Too many arguments

Warning: Wrong parameter count for asinh() in %s on line 11
NULL

Too few arguments

Warning: Wrong parameter count for asinh() in %s on line 14
NULL
