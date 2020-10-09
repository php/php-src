--TEST--
IntlCalendar::fromDateTime(): basic test
--SKIPIF--
<?php
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "nl_NL");
date_default_timezone_set('Europe/Lisbon');

$cal = IntlCalendar::fromDateTime('2012-01-01 00:00:00 Europe/Rome');
var_dump(
    $cal->getTime(),
    strtotime('2012-01-01 00:00:00 Europe/Rome') * 1000.,
    $cal->getTimeZone()->getID(),
    $cal->getLocale(1)
);
echo "\n";

$cal = IntlCalendar::fromDateTime(new DateTime('2012-01-01 00:00:00 PST'), "pt_PT");
var_dump(
    $cal->getTime(),
    strtotime('2012-01-01 00:00:00 PST') * 1000.,
    $cal->getTimeZone()->getID(),
    $cal->getLocale(1)
);

echo "\n";

$cal = intlcal_from_date_time(new DateTime('2012-01-01 00:00:00 +03:40'));
var_dump(
    $cal->getTime(),
    strtotime('2012-01-01 00:00:00 +03:40') * 1000.,
    $cal->getTimeZone()->getID()
);
?>
--EXPECTF--
float(1325372400000)
float(1325372400000)
string(11) "Europe/Rome"
string(5) "nl_NL"

float(1325404800000)
float(1325404800000)
string(3) "PST"
string(5) "pt_PT"

float(1325362800000)
float(1325362800000)
string(%d) "GMT+03%S40"
