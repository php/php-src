--TEST--
IntlDateFormatter::formatObject(): DateTime tests
--EXTENSIONS--
intl
--SKIPIF--
<?php if (version_compare(INTL_ICU_VERSION, '54.1') < 0) die('skip for ICU >= 54.1'); ?>
<?php if (version_compare(INTL_ICU_VERSION, '55.1') >=  0) die('skip for ICU < 55.1'); ?>
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "pt_PT");
ini_set("date.timezone", "Europe/Lisbon");

$dt = new DateTime('2012-01-01 00:00:00'); //Europe/Lisbon
echo IntlDateFormatter::formatObject($dt), "\n";
echo IntlDateFormatter::formatObject($dt, IntlDateFormatter::FULL), "\n";
echo IntlDateFormatter::formatObject($dt, null, "en-US"), "\n";
echo IntlDateFormatter::formatObject($dt, array(IntlDateFormatter::SHORT, IntlDateFormatter::FULL), "en-US"), "\n";
echo IntlDateFormatter::formatObject($dt, 'E y-MM-d HH,mm,ss.SSS v', "en-US"), "\n";

$dt = new DateTime('2012-01-01 05:00:00+03:00');
echo IntlDateFormatter::formatObject($dt, IntlDateFormatter::FULL), "\n";

?>
--EXPECTF--
01/01/2012, 00:00:00
domingo, 1 de janeiro de 2012 às 00:00:00 Hora Padrão %Sda Europa Ocidental
Jan 1, 2012, 12:00:00 AM
1/1/12, 12:00:00 AM Western European Standard %STime
Sun 2012-01-1 00,00,00.000 Portugal Time
domingo, 1 de janeiro de 2012 às 05:00:00 GMT+03:00
