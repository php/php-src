--TEST--
GH-16834 (cal_from_jd from julian_day argument)
--EXTENSIONS--
calendar
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip for 64bit platforms only"); ?>
--FILE--
<?php
var_dump(cal_from_jd(076545676543223, CAL_GREGORIAN));
?>
--EXPECTF--
array(9) {
  ["date"]=>
  string(5) "0/0/0"
  ["month"]=>
  int(0)
  ["day"]=>
  int(0)
  ["year"]=>
  int(0)
  ["dow"]=>
  int(%d)
  ["abbrevdayname"]=>
  string(3) "%s"
  ["dayname"]=>
  string(9) "%s"
  ["abbrevmonth"]=>
  string(0) ""
  ["monthname"]=>
  string(0) ""
}
