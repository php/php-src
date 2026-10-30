--TEST--
GH-22905: null bytes in ReflectionConstant::__construct() error messages
--FILE--
<?php

new ReflectionConstant("foo\0bar");

?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: Constant "foo%0bar" does not exist in %s:%d
Stack trace:
#0 %s(%d): ReflectionConstant->__construct('foo\x00bar')
#1 {main}
  thrown in %s on line %d
