--TEST--
IntlCalendar::clear() 1 arg variation
--INI--
date.timezone=Atlantic/Azores
--EXTENSIONS--
intl
--SKIPIF--
<?php
if (version_compare(INTL_ICU_VERSION, '73.1') >= 0 && version_compare(INTL_ICU_VERSION, '74.1') < 0) {
    die('skip Broken for ICU >= 73.1 and < 74.1, see https://github.com/php/php-src/issues/11128');
}
?>
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "nl");

$intlcal = IntlCalendar::createInstance('UTC');
$intlcal->setTime(strtotime('2012-02-29 05:06:07 +0000') * 1000);
//print_R($intlcal);
var_dump($intlcal->isSet(IntlCalendar::FIELD_MONTH));
var_dump($intlcal->clear(IntlCalendar::FIELD_MONTH));
var_dump($intlcal->isSet(IntlCalendar::FIELD_MONTH));
//print_R($intlcal);
var_dump(
    $intlcal->getTime(),
    strtotime('2012-01-29 05:06:07 +0000') * 1000.
);
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
float(1327813567000)
float(1327813567000)
