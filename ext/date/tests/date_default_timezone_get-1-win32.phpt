--TEST--
date_default_timezone_get() function [1]
--SKIPIF--
<?php
       if(substr(PHP_OS, 0, 3) != 'WIN') die("skip skip on non windows");
?>
--INI--
date.timezone=
--FILE--
<?php
       putenv('TZ=');
       echo date_default_timezone_get(), "\n";
       echo date('e'), "\n";
       /* The behaviour on windows is to select an arbitrary timezone name from the current system settings.
          This gives no chance to hardcode the timezone name, for instance for UTC+1 it could choose
          from the multiple names like Europe/Berlin or Europe/Paris . For this reason the test is
          parametrized so there is no hardcoded timezone data.*/
?>
--EXPECTF--
Warning: date_default_timezone_get(): It is not safe to rely on the system's timezone settings. You are *required* to use the date.timezone setting or the date_default_timezone_set() function. In case you used any of those methods and you are still getting this warning, you most likely misspelled the timezone identifier. We selected the timezone '%s' for now, but please set date.timezone to select your timezone. in %s on line %d
%s

Warning: date(): It is not safe to rely on the system's timezone settings. You are *required* to use the date.timezone setting or the date_default_timezone_set() function. In case you used any of those methods and you are still getting this warning, you most likely misspelled the timezone identifier. We selected the timezone '%s' for now, but please set date.timezone to select your timezone. in %s on line %d
%s
