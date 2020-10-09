--TEST--
Test return type and value for expected input microtime()
--FILE--
<?php
/*
 * Function is implemented in ext/standard/microtime.c
*/

var_dump(microtime());
var_dump(microtime(true));
var_dump(microtime(false));

?>
--EXPECTF--
string(%d) "%s %s"
float(%s)
string(%d) "%s %s"
