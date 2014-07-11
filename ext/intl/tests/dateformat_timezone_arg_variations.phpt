--TEST--
IntlDateFormatter: several forms of the timezone arg
--SKIPIF--
<?php
if (!extension_loaded('intl')) die('skip intl extension not enabled'); ?>
<?php if (version_compare(INTL_ICU_VERSION, '51.2') >=  0) die('skip for ICU < 51.2'); ?>
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("date.timezone", 'Atlantic/Azores');

$ts = strtotime('2012-01-01 00:00:00 UTC');

//should use Atlantic/Azores
$df = new IntlDateFormatter('es_ES', 0, 0, NULL);
echo $df->format($ts), "\n";

$df = new IntlDateFormatter('es_ES', 0, 0, 'Europe/Amsterdam');
echo $df->format($ts), "\n";

$df = new IntlDateFormatter('es_ES', 0, 0, new DateTimeZone('Europe/Lisbon'));
echo $df->format($ts), "\n";

$df = new IntlDateFormatter('es_ES', 0, 0, IntlTimeZone::createTimeZone('America/New_York'));
echo $df->format($ts), "\n";

//time zone has priority
$df = new IntlDateFormatter('es_ES', 0, 0, 'Europe/Amsterdam', new IntlGregorianCalendar('Europe/Lisbon'));
echo $df->format($ts), "\n";

//calendar has priority
$df = new IntlDateFormatter('es_ES', 0, 0, NULL, new IntlGregorianCalendar('Europe/Lisbon'));
echo $df->format($ts), "\n";

$df = new IntlDateFormatter('es_ES', 0, 0, 'Europe/Amsterdam', 0);
echo $df->format($ts), "\n";

--EXPECTF--
sábado%S 31 de diciembre de 2011 23:00:00 Hora%S de las Azores
domingo%S 1 de enero de 2012 01:00:00 Hora estándar de Europa Central
domingo%S 1 de enero de 2012 00:00:00 Hora%S de Europa Occidental
sábado%S 31 de diciembre de 2011 19:00:00 Hora estándar oriental
domingo%S 1 de enero de 2012 01:00:00 Hora estándar de Europa Central
domingo%S 1 de enero de 2012 00:00:00 Hora%S de Europa Occidental
domingo%S 1 de enero de 2012 01:00:00 Hora estándar de Europa Central
