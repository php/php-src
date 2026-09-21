--TEST--
GH-22905: null bytes in ReflectionProperty::skipLazyInitialization() error messages
--FILE--
<?php

$o = (object)["foo\0bar" => "baz"];
$r = new ReflectionProperty($o, "foo\0bar");
$r->skipLazyInitialization($o);

?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: Can not use skipLazyInitialization on dynamic property stdClass::$foo%0bar in %s:%d
Stack trace:
#0 %s(%d): ReflectionProperty->skipLazyInitialization(Object(stdClass))
#1 {main}
  thrown in %s on line %d
