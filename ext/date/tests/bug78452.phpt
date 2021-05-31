--TEST--
Bug #78452 (diff makes wrong in hour for Asia/Tehran)
--FILE--
<?php
date_default_timezone_set('Asia/Tehran');
$date1 = new \DateTime('2019-09-24 11:47:24');
$date2 = new \DateTime('2019-08-21 12:47:24');
var_dump($date1->diff($date2));
?>
--EXPECT--
object(DateInterval)#3 (16) {
  ["y"]=>
  int(0)
  ["m"]=>
  int(1)
  ["d"]=>
  int(2)
  ["h"]=>
  int(23)
  ["i"]=>
  int(0)
  ["s"]=>
  int(0)
  ["f"]=>
  float(0)
  ["weekday"]=>
  int(0)
  ["weekday_behavior"]=>
  int(0)
  ["first_last_day_of"]=>
  int(0)
  ["invert"]=>
  int(1)
  ["days"]=>
  int(33)
  ["special_type"]=>
  int(0)
  ["special_amount"]=>
  int(0)
  ["have_weekday_relative"]=>
  int(0)
  ["have_special_relative"]=>
  int(0)
}
