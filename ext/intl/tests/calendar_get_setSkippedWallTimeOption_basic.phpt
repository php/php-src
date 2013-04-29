--TEST--
IntlCalendar::get/setSkippedWallTimeOption(): basic test
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

//25 March 2012, transition to DST
$intlcal = new IntlGregorianCalendar(2012, 2, 25, 0, 0, 0);
var_dump($intlcal->getSkippedWallTimeOption());
$intlcal->set(IntlCalendar::FIELD_HOUR_OF_DAY, 2);
$intlcal->set(IntlCalendar::FIELD_MINUTE, 30);
echo "Should be 3h30\n";
var_dump(
	$intlcal->get(IntlCalendar::FIELD_HOUR_OF_DAY),
	$intlcal->get(IntlCalendar::FIELD_MINUTE)
);

var_dump($intlcal->setSkippedWallTimeOption(IntlCalendar::WALLTIME_FIRST));
var_dump(intlcal_get_skipped_wall_time_option($intlcal));
$intlcal->set(IntlCalendar::FIELD_HOUR_OF_DAY, 2);
$intlcal->set(IntlCalendar::FIELD_MINUTE, 30);
echo "Should be 1h30\n";
var_dump(
	$intlcal->get(IntlCalendar::FIELD_HOUR_OF_DAY),
	$intlcal->get(IntlCalendar::FIELD_MINUTE)
);

var_dump(intlcal_set_skipped_wall_time_option($intlcal, IntlCalendar::WALLTIME_NEXT_VALID));
var_dump($intlcal->getSkippedWallTimeOption());
$intlcal->set(IntlCalendar::FIELD_HOUR_OF_DAY, 2);
$intlcal->set(IntlCalendar::FIELD_MINUTE, 30);
echo "Should be 3h00\n";
var_dump(
	$intlcal->get(IntlCalendar::FIELD_HOUR_OF_DAY),
	$intlcal->get(IntlCalendar::FIELD_MINUTE)
);


?>
==DONE==
--EXPECT--

int(0)
Should be 3h30
int(3)
int(30)
bool(true)
int(1)
Should be 1h30
int(1)
int(30)
bool(true)
int(2)
Should be 3h00
int(3)
int(0)
==DONE==