--TEST--
DateTime::__toString() -- Inheritance
--CREDITS--
Thomas Rothe <th.rothe@gmail.com>
--FILE--
<?php
date_default_timezone_set("Europe/Berlin");
DateTime::setDefaultFormat('d.m.Y H:i');
$dateTime = DateTime::createFromFormat('Y-m-d H:i:s','2012-06-07 22:18:05');
echo $dateTime . PHP_EOL;
echo Datetime::getDefaultFormat() . PHP_EOL;
?>
--EXPECT--
07.06.2012 22:18
d.m.Y H:i
