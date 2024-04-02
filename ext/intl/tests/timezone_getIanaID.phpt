--TEST--
IntlTimeZone::getIanaID
--EXTENSIONS--
intl
--SKIPIF--
<?php if (version_compare(INTL_ICU_VERSION, '74.0') < 0) die('skip for ICU >= 74.0'); ?>
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(IntlTimeZone::getIanaID("php\x81"));
var_dump(IntlTimeZone::getIanaID("Galaxy/Pluto"));
var_dump(IntlTimeZone::getIanaID('Europe/Dublin'));
var_dump(IntlTimeZone::getIanaID('Asia/Calcutta'));
var_dump(intltz_get_iana_id('Asia/Calcutta') === IntlTimeZone::getIanaID('Asia/Calcutta'));
?>
--EXPECTF--
Warning: IntlTimeZone::getIanaID(): could not convert time zone id to UTF-16 in %s on line %d
bool(false)

Warning: IntlTimeZone::getIanaID(): error obtaining IANA ID in %s on line %d
bool(false)
string(13) "Europe/Dublin"
string(12) "Asia/Kolkata"
bool(true)
