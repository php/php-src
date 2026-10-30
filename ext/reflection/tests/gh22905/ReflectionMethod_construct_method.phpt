--TEST--
GH-22905: null bytes in ReflectionMethod::__construct() error messages (method name)
--FILE--
<?php

class Demo {}
new ReflectionMethod(Demo::class, "foo\0bar");

?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: Method Demo::foo%0bar() does not exist in %s:%d
Stack trace:
#0 %s(%d): ReflectionMethod->__construct('Demo', 'foo\x00bar')
#1 {main}
  thrown in %s on line %d
