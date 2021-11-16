--TEST--
Bug #71894 (AddressSanitizer: global-buffer-overflow in zif_cal_from_jd)
--EXTENSIONS--
calendar
--FILE--
<?php
var_dump(cal_from_jd(347997, CAL_JEWISH));
var_dump(jdmonthname(347997,CAL_MONTH_JEWISH));?>
--EXPECT--
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
  NULL
  ["abbrevdayname"]=>
  string(0) ""
  ["dayname"]=>
  string(0) ""
  ["abbrevmonth"]=>
  string(0) ""
  ["monthname"]=>
  string(0) ""
}
string(0) ""
