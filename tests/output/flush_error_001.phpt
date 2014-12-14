--TEST--
Test wrong number of arguments for flush() (no impact)
--FILE--
<?php
/* 
 * proto void flush(void)
 * Function is implemented in ext/standard/basic_functions.c.
 */

$extra_arg = 1;
echo "\nToo many arguments\n";
var_dump(flush($extra_arg));
?>
--EXPECTF--
Too many arguments
NULL
