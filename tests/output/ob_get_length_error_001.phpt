--TEST--
Test wrong number of arguments for ob_get_length()
--FILE--
<?php
/*
 * proto int ob_get_length(void)
 * Function is implemented in main/output.c
*/

$extra_arg = 1;

echo "\nToo many arguments\n";
var_dump(ob_get_length($extra_arg));


?>
--EXPECTF--
Too many arguments

Warning: ob_get_length() expects exactly 0 parameters, 1 given in %s on line 10
NULL
