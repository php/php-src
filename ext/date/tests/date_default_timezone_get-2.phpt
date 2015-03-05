--TEST--
date_default_timezone_get() function [2]
--SKIPIF--
<?php
	if(substr(PHP_OS, 0, 3) == 'WIN') die("skip on windows");
?>
--INI--
date.timezone=
--FILE--
<?php
	putenv('TZ=');
	echo date_default_timezone_get(), "\n";
?>
--EXPECTF--
UTC
