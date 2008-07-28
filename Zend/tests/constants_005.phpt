--TEST--
Persistent case insensetive and user defined constants
--FILE--
<?php
var_dump(ZEND_THREAD_safe);
define("ZEND_THREAD_safe", 123);
var_dump(ZEND_THREAD_safe);
?>
--EXPECTF--
bool(%s)
int(123)
