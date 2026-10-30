--TEST--
GH-22905: null bytes in ReflectionClassConstant::__construct() error messages (constant name)
--FILE--
<?php

class Demo {}
new ReflectionClassConstant(Demo::class, "foo\0bar");

?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: Constant Demo::foo%0bar does not exist in %s:%d
Stack trace:
#0 %s(%d): ReflectionClassConstant->__construct('Demo', 'foo\x00bar')
#1 {main}
  thrown in %s on line %d
