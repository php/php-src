--TEST--
Bug #49081 (DateTime::diff() mistake if start in January and interval > 28 days)
--FILE--
<?php
date_default_timezone_set('Europe/Berlin');
$d1 = new DateTime('2010-01-01 06:00:00');
$d2 = new DateTime('2010-01-31 10:00:00');
$d  = $d1->diff($d2);
var_dump($d);
?>
--EXPECTF--
object(DateTimeInterval)#%d (%d) {
  ["from_string"]=>
  bool(false)
  ["y"]=>
  int(0)
  ["m"]=>
  int(0)
  ["d"]=>
  int(30)
  ["h"]=>
  int(4)
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
}
