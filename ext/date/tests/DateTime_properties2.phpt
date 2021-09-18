--TEST--
DateTime: Test modify properties
--FILE--
<?php

$datetime = new DateTime('2000-01-02 03:04:05.100000Z');

$datetime->year = 2001;
assert($datetime->year === 2001);

$datetime->month = 2;
assert($datetime->month === 2);

$datetime->day = 3;
assert($datetime->day === 3);

$datetime->hour = 4;
assert($datetime->hour === 4);

$datetime->minute = 5;
assert($datetime->minute === 5);

$datetime->second = 6;
assert($datetime->second === 6);

$datetime->microsecond = 7;
assert($datetime->microsecond === 7);

var_dump($datetime);

?>
--EXPECT--
object(DateTime)#1 (10) {
  ["year"]=>
  int(2001)
  ["month"]=>
  int(2)
  ["day"]=>
  int(3)
  ["hour"]=>
  int(4)
  ["minute"]=>
  int(5)
  ["second"]=>
  int(6)
  ["microsecond"]=>
  int(7)
  ["date"]=>
  string(26) "2001-02-03 04:05:06.000007"
  ["timezone_type"]=>
  int(2)
  ["timezone"]=>
  string(1) "Z"
}
