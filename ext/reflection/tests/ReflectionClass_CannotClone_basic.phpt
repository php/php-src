--TEST--
Reflection class can not be cloned
--CREDITS--
Stefan Koopmanschap <stefan@phpgg.nl>
TestFest PHP|Tek
--SKIPIF--
<?php
if (!extension_loaded('reflection')) print 'skip';
?>
--FILE--
<?php
$rc = new ReflectionClass("stdClass");
$rc2 = clone($rc);
--EXPECTF--
Fatal error: Uncaught Error: Trying to clone an uncloneable object of class ReflectionClass in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
