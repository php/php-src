--TEST--
IntlTimeZone::getEquivalentID(): basic test
--EXTENSIONS--
intl
--FILE--
<?php

var_dump(IntlTimeZone::getEquivalentID('Europe/Lisbon', "1"));
var_dump(intltz_get_equivalent_id('Europe/Lisbon', 1));

?>
--EXPECT--
string(8) "Portugal"
string(8) "Portugal"
