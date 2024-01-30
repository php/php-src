--TEST--
Bug #77571 (DateTime's diff DateInterval incorrect in timezones from UTC+01:00 to UTC+12:00)
--FILE--
<?php

date_default_timezone_set('Europe/London');

$date3 = DateTime::createFromFormat('Y-m-d H:i:s', '2019-04-01 00:00:00'); //  2019-04-01 00:00:00.0 Europe/London (+01:00)
$date4 = clone $date3;
$date4->modify('+5 week'); // 2019-05-06 00:00:00.0 Europe/London (+01:00)
$differenceDateInterval2 = $date3->diff($date4); // interval: + 1m 4d; days 35
var_dump($differenceDateInterval2);
?>
--EXPECTF--
object(DateTimeInterval)#%d (%d) {
  ["from_string"]=>
  bool(false)
  ["y"]=>
  int(0)
  ["m"]=>
  int(1)
  ["d"]=>
  int(5)
  ["h"]=>
  int(0)
  ["i"]=>
  int(0)
  ["s"]=>
  int(0)
  ["f"]=>
  float(0)
  ["invert"]=>
  int(0)
  ["days"]=>
  int(35)
}
