--TEST--
Persistent case insensitive and user defined constants
--FILE--
<?php
var_dump(defined('ZEND_THREAD_safe'));
define("ZEND_THREAD_safe", 123);
var_dump(ZEND_THREAD_safe);
?>
--EXPECT--
bool(false)
int(123)
