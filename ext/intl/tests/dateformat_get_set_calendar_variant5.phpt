--TEST--
IntlDateFormatter: setCalendar()/getCalendar()/getCalendarObject() for ICU >= 58.1 and < 70.1
--EXTENSIONS--
intl
--SKIPIF--
<?php
if (version_compare(INTL_ICU_VERSION, '58.1') < 0 || version_compare(INTL_ICU_VERSION, '70.1') >= 0) die('skip for ICU >= 58.1 and < 70.1');
?>
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "pt_PT");
ini_set("date.timezone", 'Atlantic/Azores');

$ts = strtotime('2012-01-01 00:00:00 UTC');

function d(IntlDateFormatter $df) {
global $ts;
echo $df->format($ts), "\n";
var_dump($df->getCalendar(),
$df->getCalendarObject()->getType(),
$df->getCalendarObject()->getTimeZone()->getId());
echo "\n";
}

$df = new IntlDateFormatter('fr@calendar=islamic', 0, 0, 'Europe/Minsk');
d($df);


//changing the calendar with a cal type should not change tz
$df->setCalendar(IntlDateFormatter::TRADITIONAL);
d($df);

//but changing with an actual calendar should
$cal = IntlCalendar::createInstance("UTC");
$df->setCalendar($cal);
d($df);

?>
--EXPECT--
dimanche 1 janvier 2012 ap. J.-C. à 03:00:00 heure de Kaliningrad
int(1)
string(9) "gregorian"
string(12) "Europe/Minsk"

dimanche 8 safar 1433 AH à 03:00:00 heure de Kaliningrad
int(0)
string(7) "islamic"
string(12) "Europe/Minsk"

dimanche 1 janvier 2012 ap. J.-C. à 00:00:00 Temps universel coordonné
bool(false)
string(9) "gregorian"
string(3) "UTC"

