--TEST--
Comparing datetime objects with negative timestamps should account for microseconds
--FILE--
<?php

date_default_timezone_set('UTC');
$earlyDate1 = DateTime::createFromFormat('U.u', '1.8642')->modify('-5 seconds');
$earlyDate2 = DateTime::createFromFormat('U.u', '1.2768')->modify('-5 seconds');
$earlyDate3 = DateTime::createFromFormat('U.u', '1.2768')->modify('-5 seconds');

var_dump($earlyDate1 == $earlyDate2);
var_dump($earlyDate1 < $earlyDate2);
var_dump($earlyDate2 > $earlyDate1);
var_dump($earlyDate2 == $earlyDate3);
--EXPECT--
bool(false)
bool(true)
bool(true)
bool(true)
