--TEST--
IntlCalendar::setFirstDayOfWeek() basic test
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "nl");

$intlcal = IntlCalendar::createInstance('UTC');
var_dump(
		IntlCalendar::DOW_TUESDAY,
		$intlcal->setFirstDayOfWeek(IntlCalendar::DOW_TUESDAY),
		$intlcal->getFirstDayOfWeek(),
		intlcal_set_first_day_of_week($intlcal, IntlCalendar::DOW_WEDNESDAY),
		$intlcal->getFirstDayOfWeek()
);
?>
==DONE==
--EXPECT--
int(3)
bool(true)
int(3)
bool(true)
int(4)
==DONE==