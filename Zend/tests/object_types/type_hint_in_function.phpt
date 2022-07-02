--TEST--
Adding a function object type hint
--FILE--
<?php

class A {}
function a(object $obj) {}

a(new A());
a(123);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: a(): Argument #1 ($obj) must be of type object, int given, called in %s:%d
Stack trace:
#0 %s(7): a(123)
#1 {main}
  thrown in %s on line 4
