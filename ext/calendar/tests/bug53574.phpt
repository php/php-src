--TEST--
Bug #53574 (Integer overflow in SdnToJulian; leads to segfault)
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
if (PHP_INT_MAX == 0x7FFFFFFF) {
	$x = 882858043;
} else {
	$x = 3315881921229094912;
}

var_dump(cal_from_jd($x, CAL_JULIAN));
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

