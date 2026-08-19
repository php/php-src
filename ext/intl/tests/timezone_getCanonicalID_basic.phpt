--TEST--
IntlTimeZone::getCanonicalID: basic test
--EXTENSIONS--
intl
--FILE--
<?php
print_r(IntlTimeZone::getCanonicalID('Portugal'));
echo "\n";
print_r(intltz_get_canonical_id('Portugal'));
echo "\n";
?>
--EXPECT--
Europe/Lisbon
Europe/Lisbon
