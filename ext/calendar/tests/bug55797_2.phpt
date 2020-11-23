--TEST--
Bug #55797: Integer overflow in SdnToGregorian leads to segfault (in optimized builds)
--SKIPIF--
<?php
include 'skipif.inc';
if (PHP_INT_SIZE == 4) {
        die("skip this test is for 64bit platform only");
}
?>
--FILE--
<?php
$x = 9223372036854743639;

var_dump(cal_from_jd($x, CAL_GREGORIAN));
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
  string(%d) "%s"
  ["dayname"]=>
  string(%d) "%s"
  ["abbrevmonth"]=>
  string(0) ""
  ["monthname"]=>
  string(0) ""
}
