--TEST--
GH-22905: null bytes in ReflectionParameter::__construct() error messages (string function)
--FILE--
<?php

new ReflectionParameter("foo\0bar", 0);

?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: Function foo%0bar() does not exist in %s:%d
Stack trace:
#0 %s(%d): ReflectionParameter->__construct('foo\x00bar', 0)
#1 {main}
  thrown in %s on line %d
