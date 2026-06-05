--TEST--
IntlTimeZone::getTZDataVersion: basic test
--EXTENSIONS--
intl
--FILE--
<?php
print_r(IntlTimeZone::getTZDataVersion());
echo "\n";
print_r(intltz_get_tz_data_version());
echo "\n";
?>
--EXPECTF--
20%d%s
20%d%s
