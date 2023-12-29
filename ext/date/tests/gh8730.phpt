--TEST--
Bug GH-8730 (DateTime::diff miscalculation is same time zone of different type)
--FILE--
<?php
$foo = new DateTime('2022-06-08 09:15:00', new DateTimeZone('-04:00'));
$bar = new DateTime('2022-06-08 09:15:00', new DateTimeZone('US/Eastern'));
var_dump($foo->diff($bar));
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
  int(0)
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
  int(0)
}