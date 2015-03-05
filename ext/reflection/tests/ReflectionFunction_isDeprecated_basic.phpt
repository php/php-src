--TEST--
ReflectionFunction::isDeprecated
--CREDITS--
Stefan Koopmanschap <stefan@phpgg.nl>
TestFest PHP|Tek
--FILE-- 
<?php
$rc = new ReflectionFunction('magic_quotes_runtime');
var_dump($rc->isDeprecated());
--EXPECTF--
bool(true)
