--TEST--
IntlCalendar::getNow() basic test
--EXTENSIONS--
intl
--FILE--
<?php

$now = IntlCalendar::getNow();
$proc_now = intlcal_get_now();
$time = time();
var_dump(abs($now - $proc_now) < 500);
var_dump(abs($time * 1000 - $proc_now) < 2000);

?>
--EXPECT--
bool(true)
bool(true)
