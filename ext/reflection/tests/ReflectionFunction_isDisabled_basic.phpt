--TEST--
ReflectionFunction::isDisabled
--CREDITS--
Stefan Koopmanschap <stefan@phpgg.nl>
TestFest PHP|Tek
--INI--
disable_functions=is_file
--FILE--
<?php
$rc = new ReflectionFunction('is_file');
var_dump($rc->isDisabled());
?>
--EXPECT--
bool(true)
