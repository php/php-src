--TEST--
Reflection::isClosure
--CREDITS--
Stefan Koopmanschap <stefan@phpgg.nl>
TestFest PHP|Tek
--FILE--
<?php
$closure = function($param) { return "this is a closure"; };
$rc = new ReflectionFunction($closure);
var_dump($rc->isClosure());
?>
--EXPECT--
bool(true)
