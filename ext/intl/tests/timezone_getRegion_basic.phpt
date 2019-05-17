--TEST--
IntlTimeZone::getRegion(): basic test
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
print_R(IntlTimeZone::getRegion('Europe/Amsterdam'));
echo "\n";
print_R(intltz_get_region('Europe/Amsterdam'));
echo "\n";
?>
==DONE==
--EXPECT--
NL
NL
==DONE==
