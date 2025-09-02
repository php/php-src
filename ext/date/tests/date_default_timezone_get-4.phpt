--TEST--
date_default_timezone_get() function [4]
--INI--
date.timezone=Incorrect/Zone
--FILE--
<?php
    echo date_default_timezone_get(), "\n";
?>
--EXPECTF--
Warning: PHP Startup: Invalid date.timezone value 'Incorrect/Zone', using 'UTC' instead in %s on line %d
UTC
