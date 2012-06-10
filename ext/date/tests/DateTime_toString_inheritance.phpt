--TEST--
DateTime::__toString()
--CREDITS--
Thomas Rothe <th.rothe@gmail.com>
--FILE--
<?php
date_default_timezone_set("Europe/Berlin");
class GermanDate extends Datetime
{
        protected static $defaultFormat = 'd.m.Y';
}
$dateTime = new GermanDate("06/07/2012");
echo $dateTime . PHP_EOL;
echo GermanDate::getDefaultFormat() . PHP_EOL;
echo DateTime::getDefaultFormat() . PHP_EOL;
?>
--EXPECT--
07.06.2012
d.m.Y
Y-m-d\TH:i:sO