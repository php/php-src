--TEST--
Test cal_days_in_month() function : error conditions
--CREDITS--
edgarsandi - <edgar.r.sandi@gmail.com>
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
var_dump(cal_days_in_month(-1, 4, 2017));
var_dump(cal_days_in_month(CAL_GREGORIAN,0, 2009));
?>
--EXPECTF--
Warning: cal_days_in_month(): invalid calendar ID -1 in %s on line %d
bool(false)

Warning: cal_days_in_month(): invalid date in %s on line %d
bool(false)
