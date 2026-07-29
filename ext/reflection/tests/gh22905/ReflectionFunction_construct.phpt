--TEST--
GH-22905: null bytes in ReflectionFunction::__construct() error messages
--FILE--
<?php

new ReflectionFunction("foo\0bar");

?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: Function foo%0bar() does not exist in %s:%d
Stack trace:
#0 %s(%d): ReflectionFunction->__construct('foo\x00bar')
#1 {main}
  thrown in %s on line %d
