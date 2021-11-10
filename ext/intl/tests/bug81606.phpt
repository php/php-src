--TEST--
IntlDateFormatter::create() with null date and time types
--EXTENSIONS--
intl
--INI--
date.timezone=UTC
--FILE--
<?php

$ts = strtotime('2012-01-01 00:00:00 UTC');
$fmt = IntlDateFormatter::create('en_US', null, null, 'UTC');
echo $fmt->format($ts), "\n";

$fmt = new IntlDateFormatter('en_US', null, null, 'UTC');
echo $fmt->format($ts), "\n";

$fmt = datefmt_create('en_US', null, null, 'UTC');
echo $fmt->format($ts), "\n";

?>
--EXPECT--
Sunday, January 1, 2012 at 12:00:00 AM Coordinated Universal Time
Sunday, January 1, 2012 at 12:00:00 AM Coordinated Universal Time
Sunday, January 1, 2012 at 12:00:00 AM Coordinated Universal Time
