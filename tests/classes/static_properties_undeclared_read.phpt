--TEST--
Reading a non-existent static property
--FILE--
<?php
Class C {}
echo C::$p;
?>
--EXPECTF--
Fatal error: Access to undeclared static property: C::$p in %s on line 3
