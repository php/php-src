--TEST--
IntlTimeZone::getRegion(): basic test
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
print_R(IntlTimeZone::getRegion('Europe/Amsterdam'));
echo "\n";
print_R(intltz_get_region('Europe/Amsterdam'));
echo "\n";
?>
--EXPECT--
NL
NL
