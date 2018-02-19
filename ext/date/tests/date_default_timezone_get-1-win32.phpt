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
--EXPECT--
UTC
UTC
