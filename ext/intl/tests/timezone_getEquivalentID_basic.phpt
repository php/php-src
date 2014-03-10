--TEST--
IntlTimeZone::getEquivalentID(): basic test
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
print_R(IntlTimeZone::getEquivalentID('Europe/Lisbon', "1"));
echo "\n";
print_R(intltz_get_equivalent_id('Europe/Lisbon', 1));
echo "\n";
?>
==DONE==
--EXPECT--
Portugal
Portugal
==DONE==