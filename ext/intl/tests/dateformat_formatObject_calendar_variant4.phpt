--TEST--
IntlDateFormatter::formatObject(): IntlCalendar tests
--SKIPIF--
<?php
if (!extension_loaded('intl')) die('skip intl extension not enabled'); ?>
<?php if (version_compare(INTL_ICU_VERSION, '54.1') < 0) die('skip for ICU >= 54.1'); ?>
<?php if (version_compare(INTL_ICU_VERSION, '55.1') >=  0) die('skip for ICU < 55.1'); ?>
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "pt_PT");
ini_set("date.timezone", "Europe/Lisbon");

$cal = IntlCalendar::fromDateTime('2012-01-01 00:00:00'); //Europe/Lisbon
echo IntlDateFormatter::formatObject($cal), "\n";
echo IntlDateFormatter::formatObject($cal, IntlDateFormatter::FULL), "\n";
echo IntlDateFormatter::formatObject($cal, null, "en-US"), "\n";
echo IntlDateFormatter::formatObject($cal, array(IntlDateFormatter::SHORT, IntlDateFormatter::FULL), "en-US"), "\n";
echo IntlDateFormatter::formatObject($cal, 'E y-MM-d HH,mm,ss.SSS v', "en-US"), "\n";

$cal = IntlCalendar::fromDateTime('2012-01-01 05:00:00+03:00');
echo datefmt_format_object($cal, IntlDateFormatter::FULL), "\n";

$cal = IntlCalendar::createInstance(null,'en-US@calendar=islamic-civil');
$cal->setTime(strtotime('2012-01-01 00:00:00')*1000.);
echo IntlDateFormatter::formatObject($cal), "\n";
echo IntlDateFormatter::formatObject($cal, IntlDateFormatter::FULL, "en-US"), "\n";

?>
==DONE==
--EXPECTF--
01/01/2012, 00:00:00
domingo, 1 de janeiro de 2012 às 00:00:00 Hora Padrão %Sda Europa Ocidental
Jan 1, 2012, 12:00:00 AM
1/1/12, 12:00:00 AM Western European Standard %STime
Sun 2012-01-1 00,00,00.000 Portugal Time
domingo, 1 de janeiro de 2012 às 05:00:00 GMT+03:00
06/02/1433, 00:00:00
Sunday, Safar 6, 1433 at 12:00:00 AM Western European Standard Time
==DONE==
