--TEST--
Assigning a non-existent static property by reference
--FILE--
<?php
Class C {}
$a = 'foo';
C::$p =& $a;
?>
--EXPECTF--
Fatal error: Access to undeclared static property: C::$p in %s on line 4