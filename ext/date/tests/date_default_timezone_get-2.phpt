--TEST--
date_default_timezone_get() function [2]
--INI--
date.timezone=CEST
--FILE--
<?php
	putenv('TZ=');
	echo date_default_timezone_get(), "\n";
?>
--EXPECTF--
Notice: date_default_timezone_get(): Timezone setting (date.timezone) or TZ environment variable contains an unknown timezone in %sdate_default_timezone_get-2.php on line 3
UTC
