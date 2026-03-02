--TEST--
IntlTimeZone::getTZDataVersion: basic test
--EXTENSIONS--
intl
--FILE--
<?php
print_R(IntlTimeZone::getTZDataVersion());
echo "\n";
print_R(intltz_get_tz_data_version());
echo "\n";
?>
--EXPECTF--
20%d%s
20%d%s
