--TEST--
Test wrong number of arguments for time()
--FILE--
<?php
/*
 * proto int time(void)
 * Function is implemented in ext/date/php_date.c
*/

// Extra arguments are ignored
$extra_arg = 1;
echo "\nToo many arguments\n";
var_dump (time($extra_arg));
?>
===DONE===
--EXPECTF--
Too many arguments

Warning: time() expects exactly 0 parameters, 1 given in %stime_error.php on line %d
NULL
===DONE===
