--TEST--
Bug #53574 (Integer overflow in SdnToJulian; leads to segfault)
--EXTENSIONS--
calendar
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) {
        die("skip this test is for 32bit platform only");
}
?>
--FILE--
<?php
$x = 882858043;

var_dump(cal_from_jd($x, CAL_JULIAN));
?>
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
  int(5)
  ["abbrevdayname"]=>
  string(3) "Fri"
  ["dayname"]=>
  string(6) "Friday"
  ["abbrevmonth"]=>
  string(0) ""
  ["monthname"]=>
  string(0) ""
}
