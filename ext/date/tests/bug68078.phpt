--TEST--
Comparing datetime objects should account for microseconds
--FILE--
<?php

date_default_timezone_set('UTC');
$date1 = DateTime::createFromFormat('U.u', '1448889063.3531');
$date2 = DateTime::createFromFormat('U.u', '1448889063.5216');
$date3 = DateTime::createFromFormat('U.u', '1448889063.5216');

var_dump($date1 == $date2);
var_dump($date1 < $date2);
var_dump($date2 > $date1);
var_dump($date2 == $date3);
--EXPECT--
bool(false)
bool(true)
bool(true)
bool(true)
