--TEST--
date_default_timezone_get() function [1]
--INI--
date.timezone=
--FILE--
<?php
	putenv('TZ=');
	echo date_default_timezone_get(), "\n";
	echo date('e'), "\n";
?>
--EXPECTF--
Warning: date_default_timezone_get(): It is not safe to rely on the system's timezone settings. You are *required* to use the date.timezone setting or the date_default_timezone_set() function. In case you used any of those methods and you are still getting this warning, you most likely misspelled the timezone identifier. We selected 'UTC' for 'UTC/0.0/no DST' instead in %sdate_default_timezone_get-1.php on line 3
UTC

Warning: date(): It is not safe to rely on the system's timezone settings. You are *required* to use the date.timezone setting or the date_default_timezone_set() function. In case you used any of those methods and you are still getting this warning, you most likely misspelled the timezone identifier. We selected 'UTC' for 'UTC/0.0/no DST' instead in %sdate_default_timezone_get-1.php on line 4
UTC
