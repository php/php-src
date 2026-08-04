--TEST--
GH-22905: null bytes in ReflectionParameter::__construct() error messages (array class name)
--FILE--
<?php

new ReflectionParameter(["foo\0bar", ""], 0);

?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: Class "foo%0bar" does not exist in %s:%d
Stack trace:
#0 %s(%d): ReflectionParameter->__construct(Array, 0)
#1 {main}
  thrown in %s on line %d
