--TEST--
Assigning to a non-existent static property
--FILE--
<?php
Class C {}
C::$p = 1;
?>
--EXPECTF--
Fatal error: Uncaught Error: Access to undeclared static property C::$p in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line 3
