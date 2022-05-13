--TEST--
IntlTimeZone::getEquivalentID(): basic test
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
print_R(IntlTimeZone::getEquivalentID('Europe/Lisbon', "1"));
echo "\n";
print_R(intltz_get_equivalent_id('Europe/Lisbon', 1));
echo "\n";
?>
--EXPECT--
Portugal
Portugal