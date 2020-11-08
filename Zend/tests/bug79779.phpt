--TEST--
Bug #79779: Assertion failure when assigning property of string offset by reference
--FILE--
<?php
$str = "";
$str[1]->a = &$b;
?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot use string offset as an object in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
