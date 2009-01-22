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
echo "\n-- Too many arguments --\n";
var_dump (time($extra_arg));
?>
===DONE===
--EXPECTF--

-- Too many arguments --
int(%d)
===DONE===
