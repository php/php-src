--TEST--
Test wrong number of arguments for ob_end_clean()
--FILE--
<?php
/* 
 * proto bool ob_end_clean(void)
 * Function is implemented in main/output.c
*/ 

$extra_arg = 1;

echo "\nToo many arguments\n";
var_dump(ob_end_clean($extra_arg));


?>
--EXPECTF--

Too many arguments

Warning: Wrong parameter count for ob_end_clean() in %s on line 10
NULL
