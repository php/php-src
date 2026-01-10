--TEST--
IntlTimeZone::getRegion(): basic test
--EXTENSIONS--
intl
--FILE--
<?php

var_dump(IntlTimeZone::getRegion('Europe/Amsterdam'));
var_dump(intltz_get_region('Europe/Amsterdam'));

?>
--EXPECT--
string(2) "NL"
string(2) "NL"
