--TEST--
Bug #49585 (date_format buffer not long enough for >4 digit years)
--FILE--
<?php
date_default_timezone_set('UTC');

$date = new DateTime('-1500-01-01');
var_dump($date->format('r'));

$date->setDate(-2147483648, 1, 1);
var_dump($date->format('r'));
var_dump($date->format('c'));
--EXPECT--
string(32) "Sat, 01 Jan -1500 00:00:00 +0000"
string(42) "Unknown, 01 Jan -2147483648 00:00:00 +0000"
string(32) "-2147483648-01-01T00:00:00+00:00"
