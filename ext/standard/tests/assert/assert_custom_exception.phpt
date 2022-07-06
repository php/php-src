--TEST--
Throwing custom exception object from assert() throws given object
--INI--
zend.assertions = 1
assert.exception = 1
--FILE--
<?php
class CustomException extends Exception {}
assert(false, new CustomException('Exception message'));
?>
--EXPECTF--
Fatal error: Uncaught CustomException: Exception message in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
