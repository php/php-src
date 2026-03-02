--TEST--
IntlGregorianCalendar::get/setGregorianChange(): basic test
--EXTENSIONS--
intl
--INI--
date.timezone=Europe/Amsterdam
intl.default_locale=nl
--FILE--
<?php

$intlcal = new IntlGregorianCalendar();

var_dump($intlcal->getGregorianChange());

var_dump($intlcal->setGregorianChange(0));
var_dump(intlgregcal_get_gregorian_change($intlcal));

var_dump(intlgregcal_set_gregorian_change($intlcal, 1));
var_dump($intlcal->getGregorianChange());

?>
--EXPECT--
float(-12219292800000)
bool(true)
float(0)
bool(true)
float(1)
