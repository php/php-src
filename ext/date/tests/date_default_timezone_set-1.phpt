--TEST--
date_default_timezone_set() function [1]
--INI--
date.timezone=
--FILE--
<?php
	putenv("TZ=");
	$date1 = strtotime("2005-07-12 08:00:00");
	date_default_timezone_set("America/Indiana/Knox");
	$date2 = strtotime("2005-07-12 08:00:00");

	echo date_default_timezone_get(), "\n";
	echo date(DATE_ISO8601, $date1), "\n";
	echo date(DATE_ISO8601, $date2), "\n";
?>
--EXPECTF--
Strict Standards: strtotime(): It is not safe to rely on the systems timezone settings, please use the date.timezone setting, the TZ environment variable or the date_default_timezone_set() function. We use 'UTC' for 'UTC' instead. in %sdate_default_timezone_set-1.php on line 3
America/Indiana/Knox
2005-07-12T03:00:00-0500
2005-07-12T08:00:00-0500
