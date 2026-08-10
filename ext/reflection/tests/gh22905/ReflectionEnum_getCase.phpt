--TEST--
GH-22905: null bytes in ReflectionEnum::getCase() error messages
--FILE--
<?php

enum Demo {}
$r = new ReflectionEnum(Demo::class);
$r->getCase("foo\0bar");

?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: Case Demo::foo%0bar does not exist in %s:%d
Stack trace:
#0 %s(%d): ReflectionEnum->getCase('foo\x00bar')
#1 {main}
  thrown in %s on line %d
