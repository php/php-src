--TEST--
IntlCalendar::get/setRepeatedWallTimeOption(): basic test
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
if (version_compare(INTL_ICU_VERSION, '49') < 0)
	die('skip for ICU 49+');
--FILE--

<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "nl");

date_default_timezone_set('Europe/Amsterdam');

//28 October 2012, transition from DST
$intlcal = new IntlGregorianCalendar(2012, 9, 28, 0, 0, 0);
var_dump($intlcal->setRepeatedWallTimeOption(IntlCalendar::WALLTIME_LAST));
var_dump($intlcal->getRepeatedWallTimeOption());
$intlcal->set(IntlCalendar::FIELD_HOUR_OF_DAY, 2);
$intlcal->set(IntlCalendar::FIELD_MINUTE, 30);
var_dump(
	strtotime('2012-10-28 02:30:00 +0100'),
	(int)($intlcal->getTime() /1000)
);

var_dump(intlcal_set_repeated_wall_time_option($intlcal, IntlCalendar::WALLTIME_FIRST));
var_dump(intlcal_get_repeated_wall_time_option($intlcal));
$intlcal->set(IntlCalendar::FIELD_HOUR_OF_DAY, 2);
$intlcal->set(IntlCalendar::FIELD_MINUTE, 30);
var_dump(
	strtotime('2012-10-28 02:30:00 +0200'),
	(int)($intlcal->getTime() /1000)
);

?>
==DONE==
--EXPECT--

bool(true)
int(0)
int(1351387800)
int(1351387800)
bool(true)
int(1)
int(1351384200)
int(1351384200)
==DONE==