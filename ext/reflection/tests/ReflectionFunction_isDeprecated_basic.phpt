--TEST--
ReflectionFunction::isDeprecated
--CREDITS--
Stefan Koopmanschap <stefan@phpgg.nl>
TestFest PHP|Tek
--SKIPIF--
<?php

if (!extension_loaded('mcrypt')) echo "skip no deprecated functions available";

?>
--FILE-- 
<?php
$rc = new ReflectionFunction('mcrypt_ecb');
var_dump($rc->isDeprecated());
--EXPECTF--
bool(true)
