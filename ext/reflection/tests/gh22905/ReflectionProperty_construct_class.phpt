--TEST--
GH-22905: null bytes in ReflectionProperty::__construct() error messages (class name)
--FILE--
<?php

new ReflectionProperty("foo\0bar", "");

?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: Class "foo%0bar" does not exist in %s:%d
Stack trace:
#0 %s(%d): ReflectionProperty->__construct('foo\x00bar', '')
#1 {main}
  thrown in %s on line %d
