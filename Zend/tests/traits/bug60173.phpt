--TEST--
Bug #60173 (Wrong error message on reflective trait instantiation)
--FILE--
<?php

trait foo { }

$rc = new ReflectionClass('foo');
$rc->newInstance();
?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: Class foo cannot be instantiated manually in %s:%d
Stack trace:
#0 %s(%d): ReflectionClass->newInstance()
#1 {main}
  thrown in %s on line %d
