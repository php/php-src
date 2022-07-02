--TEST--
IntlTimeZone::getTZDataVersion: basic test
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
print_R(IntlTimeZone::getTZDataVersion());
echo "\n";
print_R(intltz_get_tz_data_version());
echo "\n";
?>
--EXPECTF--
20%d%s
20%d%s