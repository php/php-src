--TEST--
Reflection::isClosure
--CREDITS--
Stefan Koopmanschap <stefan@phpgg.nl>
TestFest PHP|Tek
--SKIPIF--
<?php
if (!extension_loaded('reflection') || !defined('PHP_VERSION_ID') || PHP_VERSION_ID < 50300) {
  print 'skip';
}
?>
--FILE-- 
<?php
$closure = function($param) { return "this is a closure"; };
$rc = new ReflectionFunction($closure);
echo var_dump($rc->isClosure());
--EXPECTF--
bool(true)
