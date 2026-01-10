--TEST--
IntlCalendar::get() basic test
--EXTENSIONS--
intl
--FILE--
<?php

$intlcal = IntlCalendar::createInstance('UTC');
$intlcal->set(IntlCalendar::FIELD_DAY_OF_MONTH, 4);

var_dump($intlcal->get(IntlCalendar::FIELD_DAY_OF_MONTH));
var_dump(intlcal_get($intlcal, IntlCalendar::FIELD_DAY_OF_MONTH));

?>
--EXPECT--
int(4)
int(4)
