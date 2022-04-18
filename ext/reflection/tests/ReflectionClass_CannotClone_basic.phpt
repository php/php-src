--TEST--
Reflection class cannot be cloned
--CREDITS--
Stefan Koopmanschap <stefan@phpgg.nl>
TestFest PHP|Tek
--FILE--
<?php
$rc = new ReflectionClass("stdClass");
$rc2 = clone($rc);
?>
--EXPECTF--
Fatal error: Uncaught Error: Trying to clone an uncloneable object of class ReflectionClass in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
