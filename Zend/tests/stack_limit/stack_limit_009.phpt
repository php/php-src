--TEST--
Stack limit 009 - Check that we can actually use all the stack
--SKIPIF--
<?php
if (!function_exists('zend_test_zend_call_stack_get')) die("skip zend_test_zend_call_stack_get() is not available");
if (getenv('SKIP_MSAN')) die("skip msan requires a considerably higher zend.reserved_stack_size due to instrumentation");
?>
--EXTENSIONS--
zend_test
--FILE--
<?php

var_dump(zend_test_zend_call_stack_get());

var_dump(zend_test_zend_call_stack_use_all());

?>
--EXPECTF--
array(4) {
  ["base"]=>
  string(%d) "0x%x"
  ["max_size"]=>
  string(%d) "0x%x"
  ["position"]=>
  string(%d) "0x%x"
  ["EG(stack_limit)"]=>
  string(%d) "0x%x"
}
int(%d)
