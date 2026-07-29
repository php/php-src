--TEST--
GH-22905: null bytes in ReflectionMethod::createFromMethodName() error messages (method name)
--FILE--
<?php

class Demo {}
ReflectionMethod::createFromMethodName("Demo::foo\0bar");

?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: Method Demo::foo%0bar() does not exist in %s:%d
Stack trace:
#0 %s(%d): ReflectionMethod::createFromMethodName('Demo::foo\x00bar')
#1 {main}
  thrown in %s on line %d
