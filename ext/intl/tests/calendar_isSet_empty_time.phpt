--TEST--
Test IntlCalendar::setDateTime()
--EXTENSIONS--
intl
--FILE--
<?php

$intlcal = IntlCalendar::createInstance('UTC');
$intlcal->clear();

$intlcal->set(2012, 1, 29, 23, 58);
var_dump($intlcal->isSet(IntlCalendar::FIELD_SECOND));
var_dump($intlcal->get(IntlCalendar::FIELD_MINUTE));
var_dump($intlcal->isSet(IntlCalendar::FIELD_SECOND));

$intlcal->clear();

$intlcal->setDateTime(2012, 1, 29, 23, 58);
var_dump($intlcal->isSet(IntlCalendar::FIELD_SECOND));
var_dump($intlcal->get(IntlCalendar::FIELD_MINUTE));
var_dump($intlcal->isSet(IntlCalendar::FIELD_SECOND));

?>
--EXPECT--
bool(false)
int(58)
bool(true)
bool(false)
int(58)
bool(true)
