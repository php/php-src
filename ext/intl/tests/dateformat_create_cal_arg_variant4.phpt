--TEST--
IntlDateFormatter: several forms of the calendar arg
--EXTENSIONS--
intl
--SKIPIF--
<?php if (version_compare(INTL_ICU_VERSION, '54.1') < 0) die('skip for ICU >= 54.1'); ?>
<?php if (version_compare(INTL_ICU_VERSION, '58.1') >= 0) die('skip for ICU <= 57.1'); ?>
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "pt_PT");
ini_set("date.timezone", 'Atlantic/Azores');

$ts = strtotime('2012-01-01 00:00:00 UTC');

$cal = new IntlGregorianCalendar('UTC', NULL);
$df = new IntlDateFormatter('es_ES', 0, 0, NULL, $cal);
echo $df->format($ts), "\n";

$cal = IntlCalendar::createInstance('UTC', 'en@calendar=islamic');
$df = new IntlDateFormatter('es_ES', 0, 0, NULL, $cal);
echo $df->format($ts), "\n";

//override calendar's timezone
$cal = new IntlGregorianCalendar('UTC', NULL);
$df = new IntlDateFormatter('es_ES', 0, 0, 'Europe/Madrid', $cal);
echo $df->format($ts), "\n";

//default calendar is gregorian
$df = new IntlDateFormatter('es_ES@calendar=islamic', 0, 0);
echo $df->format($ts), "\n";

//try now with traditional
$df = new IntlDateFormatter('es_ES@calendar=islamic', 0, 0, NULL, IntlDateFormatter::TRADITIONAL);
echo $df->format($ts), "\n";

//the timezone can be overridden when not specifying a calendar
$df = new IntlDateFormatter('es_ES@calendar=islamic', 0, 0, 'UTC', IntlDateFormatter::TRADITIONAL);
echo $df->format($ts), "\n";

$df = new IntlDateFormatter('es_ES', 0, 0, 'UTC', 0);
echo $df->format($ts), "\n";

?>
--EXPECTF--
domingo%S 1 de enero de 2012, 0:00:00 (GMT)
domingo%S 8 de Safar de 1433, 0:00:00 (GMT)
domingo, 1 de enero de 2012, 1:00:00 (hora estándar de Europa central)
sábado, 31 de diciembre de 2011 d. C., 23:00:00 (hora estándar de las Azores)
sábado, 7 de Safar de 1433 AH, 23:00:00 (hora estándar de las Azores)
domingo%S 8 de Safar de 1433 AH, 0:00:00 (GMT)
domingo%S 1 de enero de 2012, 0:00:00 (GMT)
