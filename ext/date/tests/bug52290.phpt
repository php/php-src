--TEST--
Bug #52290 (setDate, setISODate, setTime works wrong when DateTime created from timestamp)
--FILE--
<?php
$tz = 'UTC';
date_default_timezone_set($tz);

$ts = strtotime('2006-01-01');
$dt = new DateTime('@'.$ts);
$dt->setTimezone(new DateTimeZone($tz));

var_dump($dt->format('o-\WW-N | Y-m-d | H:i:s | U'));

$dt->setISODate(2005, 52, 1);
var_dump($dt->format('o-\WW-N | Y-m-d | H:i:s | U'));

$dt->setDate(2007, 10, 10);
var_dump($dt->format('o-\WW-N | Y-m-d | H:i:s | U'));

$dt->setTime(20, 30, 40);
var_dump($dt->format('o-\WW-N | Y-m-d | H:i:s | U'));
?>
--EXPECT--
string(47) "2005-W52-7 | 2006-01-01 | 00:00:00 | 1136073600"
string(47) "2005-W52-1 | 2005-12-26 | 00:00:00 | 1135555200"
string(47) "2007-W41-3 | 2007-10-10 | 00:00:00 | 1191974400"
string(47) "2007-W41-3 | 2007-10-10 | 20:30:40 | 1192048240"
