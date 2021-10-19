--TEST--
Bug #79779: Assertion failure when assigning property of string offset by reference
--FILE--
<?php
$str = "";
$str[1]->a = &$b;
?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot indirectly modify string offset in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
