--TEST--
IntlTimeZone::getCanonicalID: basic test
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
print_R(IntlTimeZone::getCanonicalID('Portugal'));
echo "\n";
print_R(intltz_get_canonical_id('Portugal'));
echo "\n";
?>
==DONE==
--EXPECT--
Europe/Lisbon
Europe/Lisbon
==DONE==