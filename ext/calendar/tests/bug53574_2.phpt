--TEST--
Bug #53574 (Integer overflow in SdnToJulian; leads to segfault)
--EXTENSIONS--
calendar
--SKIPIF--
<?php
if (PHP_INT_SIZE == 4) {
        die("skip this test is for 64bit platform only");
}
?>
--FILE--
<?php
$x = 3315881921229094912;

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
  int(3)
  ["abbrevdayname"]=>
  string(3) "Wed"
  ["dayname"]=>
  string(9) "Wednesday"
  ["abbrevmonth"]=>
  string(0) ""
  ["monthname"]=>
  string(0) ""
}
