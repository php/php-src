--TEST--
Test return type and value for expected input microtime()
--FILE--
<?php
/* 
 * proto mixed microtime([bool get_as_float])
 * Function is implemented in ext/standard/microtime.c
*/ 

var_dump(microtime());
var_dump(microtime(true));
var_dump(microtime(false));

?>
===DONE===
--EXPECTF--
unicode(%d) "%s %s"
float(%s)
unicode(%d) "%s %s"
===DONE===
