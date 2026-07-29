--TEST--
GH-22905: null bytes in ReflectionZendExtension::__construct() error messages
--FILE--
<?php

new ReflectionZendExtension("foo\0bar");

?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: Zend Extension "foo%0bar" does not exist in %s:%d
Stack trace:
#0 %s(%d): ReflectionZendExtension->__construct('foo\x00bar')
#1 {main}
  thrown in %s on line %d
