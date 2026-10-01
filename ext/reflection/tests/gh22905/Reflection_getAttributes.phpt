--TEST--
GH-22905: null bytes in Reflection*::getAttributes() error messages
--FILE--
<?php

class Demo {}
$r = new ReflectionClass(Demo::class);
$r->getAttributes("foo\0bar", ReflectionAttribute::IS_INSTANCEOF);

?>
--EXPECTF--
Fatal error: Uncaught Error: Class "foo%0bar" not found in %s:%d
Stack trace:
#0 %s(%d): ReflectionClass->getAttributes('foo\x00bar', 2)
#1 {main}
  thrown in %s on line %d
