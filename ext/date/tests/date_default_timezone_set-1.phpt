--TEST--
date_default_timezone_set() function [1]
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') die('skip diff TZ detection on windows.');
?>
--INI--
date.timezone=
--FILE--
<?php
    putenv("TZ=");
    $date1 = strtotime("2005-01-12 08:00:00");
    $date2 = strtotime("2005-07-12 08:00:00");
    date_default_timezone_set("America/Indiana/Knox");
    $date3 = strtotime("2005-01-12 08:00:00");
    $date4 = strtotime("2005-07-12 08:00:00");

    echo date_default_timezone_get(), "\n";
    echo date(DATE_ISO8601, $date1), "\n";
    echo date(DATE_ISO8601, $date2), "\n";
    echo date(DATE_ISO8601, $date3), "\n";
    echo date(DATE_ISO8601, $date4), "\n";
?>
--EXPECTF--
Warning: PHP Startup: Invalid date.timezone value '', using 'UTC' instead in %s on line %d
America/Indiana/Knox
2005-01-12T03:00:00-0500
2005-07-12T03:00:00-0500
2005-01-12T08:00:00-0500
2005-07-12T08:00:00-0500
