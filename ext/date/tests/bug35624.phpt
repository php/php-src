--TEST--
Bug #35624 (strtotime() does not handle 3 character weekdays)
--FILE--
<?php
date_default_timezone_set("UTC");

$days = array("monday","mon","tuesday","tue","wednesday","wed","thursday","thu","friday","fri","saturday","sat","sunday","sun");

foreach ($days as $day) {
	echo date("D", strtotime($day));
	echo date("D", strtotime(ucfirst($day)));
	echo "\n";
}
?>
--EXPECT--
MonMon
MonMon
TueTue
TueTue
WedWed
WedWed
ThuThu
ThuThu
FriFri
FriFri
SatSat
SatSat
SunSun
SunSun
