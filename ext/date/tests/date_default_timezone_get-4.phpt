--TEST--
date_default_timezone_get() function [4]
--INI--
date.timezone=Incorrect/Zone
--FILE--
<?php
    echo date_default_timezone_get(), "\n";
?>
--EXPECTF--
Warning: date_default_timezone_get(): UTC was used as timezone, because the date.timezone value 'Incorrect/Zone' is invalid in %sdate_default_timezone_get-4.php on line %d
UTC
