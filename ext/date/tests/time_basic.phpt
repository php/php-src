--TEST--
Test return type and value for expected input time()
--FILE--
<?php
/* 
 * proto int time(void)
 * Function is implemented in ext/date/php_date.c
*/ 

var_dump(time());

?>
===DONE===
--EXPECTF--
int(%d)
===DONE===