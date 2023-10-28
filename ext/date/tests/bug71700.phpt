--TEST--
Bug #71700 (Extra day on diff between begin and end of march 2016)
--INI--
date.timezone=Europe/Paris
--FILE--
<?php
$date1 = new \DateTime('2016-03-01');
$date2 = new \DateTime('2016-03-31');

$diff = date_diff($date1, $date2, true);

var_dump($diff);
?>
--EXPECTF--
object(DateInterval)#3 (%d) {
  ["y"]=>
  int(0)
  ["m"]=>
  int(0)
  ["d"]=>
  int(30)
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
  int(30)
  ["from_string"]=>
  bool(false)
  ["date_string"]=>
  NULL
}
