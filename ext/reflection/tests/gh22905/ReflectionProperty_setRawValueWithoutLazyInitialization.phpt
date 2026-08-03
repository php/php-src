--TEST--
GH-22905: null bytes in ReflectionProperty::setRawValueWithoutLazyInitialization() error messages
--FILE--
<?php

$o = (object)["foo\0bar" => "baz"];
$r = new ReflectionProperty($o, "foo\0bar");
$r->setRawValueWithoutLazyInitialization($o, 123);

?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: Can not use setRawValueWithoutLazyInitialization on dynamic property stdClass::$foo%0bar in %s:%d
Stack trace:
#0 %s(%d): ReflectionProperty->setRawValueWithoutLazyInitialization(Object(stdClass), 123)
#1 {main}
  thrown in %s on line %d
