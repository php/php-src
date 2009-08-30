--TEST--
ReflectionFunction::isDeprecated
--CREDITS--
Stefan Koopmanschap <stefan@phpgg.nl>
TestFest PHP|Tek
--SKIPIF--
<?php
if (!extension_loaded('reflection') || !defined('PHP_VERSION_ID') || PHP_VERSION_ID < 50300) print 'skip';
?>
--FILE-- 
<?php
$rc = new ReflectionFunction('ereg');
echo var_dump($rc->isDeprecated());
--EXPECTF--
bool(true)
