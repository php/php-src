--TEST--
Bug #78452 (diff makes wrong in hour for Asia/Tehran)
--FILE--
<?php
date_default_timezone_set('Asia/Tehran');
$date1 = new \DateTime('2019-09-24 11:47:24');
$date2 = new \DateTime('2019-08-21 12:47:24');
var_dump($date1->diff($date2));
?>
--EXPECTF--
object(DateInterval)#3 (%d) {
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
  ["invert"]=>
  int(1)
  ["days"]=>
  int(33)
  ["from_string"]=>
  bool(false)
}
