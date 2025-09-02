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
--EXPECT--
Warning: PHP Startup: Invalid date.timezone value '', using 'UTC' instead in Unknown on line 0
UTC
