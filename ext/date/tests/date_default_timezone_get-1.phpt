--TEST--
date_default_timezone_get() function [1]
--INI--
date.timezone=
--FILE--
<?php
	putenv('TZ=');
	echo date_default_timezone_get(), "\n";
?>
--EXPECT--
Strict Standards: date_default_timezone_get(): It is not safe to rely on the systems timezone settings, please use the date.timezone setting, the TZ environment variable or the date_default_timezone_set() function. We use 'UTC' for 'UTC' instead. in /dat/dev/php/php-5.1dev/ext/date/tests/date_default_timezone_get-1.php on line 3
UTC
