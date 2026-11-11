--TEST--
Timeout while EG(exception) is already set
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--INI--
max_execution_time=1
--FILE--
<?php

zend_test_timeout_throw();

?>
--EXPECTF--
Warning: Uncaught Exception: Timeout in %s:%d
Stack trace:
#0 %s(%d): zend_test_timeout_throw()
#1 {main}
  thrown in %s on line %d

Fatal error: Maximum execution time of 1 second exceeded in %stimeout_variation_15.php on line 3
