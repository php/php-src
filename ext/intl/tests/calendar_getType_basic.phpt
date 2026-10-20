--TEST--
IntlCalendar::getType() basic test
--EXTENSIONS--
intl
--FILE--
<?php

$intlcal = IntlCalendar::createInstance();
var_dump($intlcal->getType());
$intlcal = IntlCalendar::createInstance(null, "nl_NL@calendar=hebrew");
var_dump(intlcal_get_type($intlcal));
?>
--EXPECT--
string(9) "gregorian"
string(6) "hebrew"
