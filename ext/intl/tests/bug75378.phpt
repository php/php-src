--TEST--
Bug #75378 ([REGRESSION] IntlDateFormatter::parse() does not change $position argument)
--SKIPIF--
<?php
if (!extension_loaded('intl')) die('skip this test requires ext/intl');
?>
--FILE--
<?php

$formatter = new IntlDateFormatter("en-GB", IntlDateFormatter::NONE, IntlDateFormatter::NONE, 'UTC', null, 'yyyy-MM-dd');
$position = 0;
$parsedDate = $formatter->parse("2017-10-12", $position);
var_dump($parsedDate, $position);
$localdate = $formatter->localtime("2017-10-12 00:00:00", $position1);
var_dump($localdate, $position1);
?>
--EXPECT--
int(1507766400)
int(10)
array(9) {
  ["tm_sec"]=>
  int(0)
  ["tm_min"]=>
  int(0)
  ["tm_hour"]=>
  int(0)
  ["tm_year"]=>
  int(117)
  ["tm_mday"]=>
  int(12)
  ["tm_wday"]=>
  int(4)
  ["tm_yday"]=>
  int(285)
  ["tm_mon"]=>
  int(9)
  ["tm_isdst"]=>
  int(0)
}
int(10)
