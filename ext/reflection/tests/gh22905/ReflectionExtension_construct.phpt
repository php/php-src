--TEST--
GH-22905: null bytes in ReflectionExtension::__construct() error messages
--FILE--
<?php

new ReflectionExtension("foo\0bar");

?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: Extension "foo%0bar" does not exist in %s:%d
Stack trace:
#0 %s(%d): ReflectionExtension->__construct('foo\x00bar')
#1 {main}
  thrown in %s on line %d
