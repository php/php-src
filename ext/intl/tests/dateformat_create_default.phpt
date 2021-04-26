--TEST--
IntlDateFormatter::create() with default date and time types
--EXTENSIONS--
intl
--FILE--
<?php

$ts = strtotime('2012-01-01 00:00:00 UTC');
$fmt = IntlDateFormatter::create('en_US');
echo $fmt->format($ts), "\n";

$fmt = new IntlDateFormatter('en_US');
echo $fmt->format($ts), "\n";

$fmt = datefmt_create('en_US');
echo $fmt->format($ts), "\n";

?>
--EXPECT--
Sunday, January 1, 2012 at 12:00:00 AM Coordinated Universal Time
Sunday, January 1, 2012 at 12:00:00 AM Coordinated Universal Time
Sunday, January 1, 2012 at 12:00:00 AM Coordinated Universal Time
