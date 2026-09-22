--TEST--
GH-22905: null bytes in ReflectionClassConstant::__construct() error messages (class name)
--FILE--
<?php

new ReflectionClassConstant("foo\0bar", "");

?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: Class "foo%0bar" does not exist in %s:%d
Stack trace:
#0 %s(%d): ReflectionClassConstant->__construct('foo\x00bar', '')
#1 {main}
  thrown in %s on line %d
