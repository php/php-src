--TEST--
date() format params
--FILE--
<?php

date_default_timezone_set("UTC");
$tz = array("UTC", "Asia/Jerusalem", "America/Chicago", "Europe/London");
$t = mktime(0, 0, 0, 6, 27, 2006);

foreach ($tz as $zone) {
	date_default_timezone_set($zone);

	var_dump(date("w", $t));
	var_dump(date("z", $t));
	var_dump(date("n", $t));
	var_dump(date("t", $t));
	var_dump(date("L", $t));
	var_dump(date("a", $t));
	var_dump(date("B", $t));
	var_dump(date("g", $t));
	var_dump(date("G", $t));
	var_dump(date("Z", $t));
	var_dump(date("U", $t));
}

echo "Done\n";
?>
--EXPECTF--
unicode(1) "2"
unicode(3) "177"
unicode(1) "6"
unicode(2) "30"
unicode(1) "0"
unicode(2) "am"
unicode(3) "041"
unicode(2) "12"
unicode(1) "0"
unicode(1) "0"
unicode(10) "1151366400"
unicode(1) "2"
unicode(3) "177"
unicode(1) "6"
unicode(2) "30"
unicode(1) "0"
unicode(2) "am"
unicode(3) "041"
unicode(1) "3"
unicode(1) "3"
unicode(5) "10800"
unicode(10) "1151366400"
unicode(1) "1"
unicode(3) "176"
unicode(1) "6"
unicode(2) "30"
unicode(1) "0"
unicode(2) "pm"
unicode(3) "041"
unicode(1) "7"
unicode(2) "19"
unicode(6) "-18000"
unicode(10) "1151366400"
unicode(1) "2"
unicode(3) "177"
unicode(1) "6"
unicode(2) "30"
unicode(1) "0"
unicode(2) "am"
unicode(3) "041"
unicode(1) "1"
unicode(1) "1"
unicode(4) "3600"
unicode(10) "1151366400"
Done
