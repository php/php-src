--TEST--
IntlTimeZone::getRegion(): basic test
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
if (version_compare(INTL_ICU_VERSION, '4.8') < 0)
	die('skip for ICU 4.8+');
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