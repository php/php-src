--TEST--
IntlTimeZone::getIanaID
--EXTENSIONS--
intl
--SKIPIF--
<?php if (version_compare(INTL_ICU_VERSION, '74.0') < 0) die('skip for ICU >= 74.0'); ?>
--FILE--
<?php

var_dump(IntlTimeZone::getIanaID("php\x81"));
echo intl_get_error_message(), PHP_EOL;
var_dump(IntlTimeZone::getIanaID("Galaxy/Pluto"));
echo intl_get_error_message(), PHP_EOL;
var_dump(IntlTimeZone::getIanaID('Europe/Dublin'));
echo intl_get_error_message(), PHP_EOL;
var_dump(IntlTimeZone::getIanaID('Asia/Calcutta'));
echo intl_get_error_message(), PHP_EOL;
var_dump(intltz_get_iana_id('Asia/Calcutta') === IntlTimeZone::getIanaID('Asia/Calcutta'));
?>
--EXPECT--
bool(false)
IntlTimeZone::getIanaID(): could not convert time zone id to UTF-16: U_INVALID_CHAR_FOUND
bool(false)
IntlTimeZone::getIanaID(): error obtaining IANA ID: U_ILLEGAL_ARGUMENT_ERROR
string(13) "Europe/Dublin"
U_ZERO_ERROR
string(12) "Asia/Kolkata"
U_ZERO_ERROR
bool(true)
