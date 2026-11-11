--TEST--
Timeout within internal function
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--INI--
max_execution_time=1
--FILE--
<?php

zend_test_timeout();

?>
--EXPECTF--
Fatal error: Maximum execution time of 1 second exceeded in %stimeout_variation_14.php on line 3
