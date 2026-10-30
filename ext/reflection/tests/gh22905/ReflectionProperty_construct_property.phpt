--TEST--
GH-22905: null bytes in ReflectionProperty::__construct() error messages (property name)
--FILE--
<?php

class Demo {}
new ReflectionProperty(Demo::class, "foo\0bar");

?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: Property Demo::$foo%0bar does not exist in %s:%d
Stack trace:
#0 %s(%d): ReflectionProperty->__construct('Demo', 'foo\x00bar')
#1 {main}
  thrown in %s on line %d
