--TEST--
Reading a non-existent static property
--FILE--
<?php
Class C {}
echo C::$p;
?>
--EXPECTF--
Fatal error: Uncaught Error: Access to undeclared static property C::$p in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line 3
