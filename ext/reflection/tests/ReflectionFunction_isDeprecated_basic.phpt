--TEST--
ReflectionFunction::isDeprecated
--CREDITS--
Stefan Koopmanschap <stefan@phpgg.nl>
TestFest PHP|Tek
--FILE--
<?php
// We currently don't have any deprecated functions :/
$rc = new ReflectionFunction('var_dump');
var_dump($rc->isDeprecated());
?>
--EXPECT--
bool(false)
