--TEST--
Persistent case insensitive and user defined constants
--FILE--
<?php
var_dump(ZEND_THREAD_safe);
define("ZEND_THREAD_safe", 123);
var_dump(ZEND_THREAD_safe);
?>
--EXPECTF--
Warning: Use of undefined constant ZEND_THREAD_safe - assumed 'ZEND_THREAD_safe' (this will throw an Error in a future version of PHP) in %s on line %d
string(16) "ZEND_THREAD_safe"
int(123)
