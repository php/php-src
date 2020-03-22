--TEST--
Inexistent class as type receiving scalar argument
--FILE--
<?php

function foo(bar $ex) {}
foo(null);

?>
--EXPECTF--
Fatal error: Uncaught TypeError: foo(): Argument #1 ($ex) must be of type bar, null given, called in %s:%d
Stack trace:
#0 %s(%d): foo(NULL)
#1 {main}
  thrown in %s on line %d
