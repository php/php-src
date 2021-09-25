--TEST--
IntlTimeZone::getCanonicalID: basic test
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
print_R(IntlTimeZone::getCanonicalID('Portugal'));
echo "\n";
print_R(intltz_get_canonical_id('Portugal'));
echo "\n";
?>
--EXPECT--
Europe/Lisbon
Europe/Lisbon