--TEST--
Adding a function object type hint
--FILE--
<?php

class A {}
function a(object $obj) {}

a(new A());
a(123);
--EXPECTF--
Fatal error: Uncaught TypeError: Argument 1 passed to a() must be an object, integer given, called in %s.php on line 7 and defined in %s:4
Stack trace:
#0 %s(7): a(123)
#1 {main}
  thrown in %s on line 4
