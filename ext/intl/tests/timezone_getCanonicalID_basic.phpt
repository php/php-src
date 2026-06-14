--TEST--
IntlTimeZone::getCanonicalID: basic test
--EXTENSIONS--
intl
--FILE--
<?php
print_R(IntlTimeZone::getCanonicalID('Portugal'));
echo "\n";
print_R(intltz_get_canonical_id('Portugal'));
echo "\n";
?>
--EXPECT--
Europe/Lisbon
Europe/Lisbon
