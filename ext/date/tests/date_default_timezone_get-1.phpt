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
Strict Standards: date_default_timezone_get(): It is not safe to rely on the systems timezone settings, please use the date.timezone setting, the TZ environment variable or the date_default_timezone_set() function. We use 'Europe/London' for 'UTC/0.0/no DST' instead. in %sdate_default_timezone_get-1.php on line 3
Europe/London

Strict Standards: date(): It is not safe to rely on the systems timezone settings, please use the date.timezone setting, the TZ environment variable or the date_default_timezone_set() function. We use 'Europe/London' for 'UTC/0.0/no DST' instead. in %sdate_default_timezone_get-1.php on line 4
Europe/London
