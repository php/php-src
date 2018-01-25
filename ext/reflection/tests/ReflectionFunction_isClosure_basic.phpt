--TEST--
Reflection::isClosure
--CREDITS--
Stefan Koopmanschap <stefan@phpgg.nl>
TestFest PHP|Tek
--SKIPIF--
<?php
if (!extension_loaded('reflection')) print 'skip';
?>
--FILE--
<?php
$closure = function($param) { return "this is a closure"; };
$rc = new ReflectionFunction($closure);
var_dump($rc->isClosure());
--EXPECTF--
bool(true)
