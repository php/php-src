--TEST--
Test return type and value for expected input time()
--FILE--
<?php
/*
 * Function is implemented in ext/date/php_date.c
*/

var_dump(time());

?>
--EXPECTF--
int(%d)
