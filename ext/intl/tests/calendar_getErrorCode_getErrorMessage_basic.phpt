--TEST--
IntlCalendar::getErrorCode(), ::getErrorMessage() basic test
--INI--
date.timezone=Atlantic/Azores
intl.default_locale=nl
--EXTENSIONS--
intl
--FILE--
<?php

$intlcal = IntlGregorianCalendar::createFromDate(2012, 1, 29);
var_dump(
	$intlcal->getErrorCode(),
	intlcal_get_error_code($intlcal),
	$intlcal->getErrorMessage(),
	intlcal_get_error_message($intlcal)
);
$intlcal->add(IntlCalendar::FIELD_SECOND, 2147483647);
$intlcal->fieldDifference(-PHP_INT_MAX, IntlCalendar::FIELD_SECOND);

var_dump(
	$intlcal->getErrorCode(),
	intlcal_get_error_code($intlcal),
	$intlcal->getErrorMessage(),
	intlcal_get_error_message($intlcal)
);
?>
--EXPECT--
int(0)
int(0)
string(12) "U_ZERO_ERROR"
string(12) "U_ZERO_ERROR"
int(1)
int(1)
string(88) "IntlCalendar::fieldDifference(): Call to ICU method has failed: U_ILLEGAL_ARGUMENT_ERROR"
string(88) "IntlCalendar::fieldDifference(): Call to ICU method has failed: U_ILLEGAL_ARGUMENT_ERROR"
