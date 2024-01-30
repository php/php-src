--TEST--
Bug #48678 (DateInterval segfaults when unserialising)
--FILE--
<?php
$x = new DateInterval("P3Y6M4DT12H30M5S");
var_dump($x);
$y = unserialize(serialize($x));
var_dump($y);
?>
--EXPECTF--
object(DateInterval)#%d (%d) {
  ["from_string"]=>
  bool(false)
  ["y"]=>
  int(3)
  ["m"]=>
  int(6)
  ["d"]=>
  int(4)
  ["h"]=>
  int(12)
  ["i"]=>
  int(30)
  ["s"]=>
  int(5)
  ["f"]=>
  float(0)
  ["invert"]=>
  int(0)
  ["days"]=>
  bool(false)
}
object(DateInterval)#%d (%d) {
  ["from_string"]=>
  bool(false)
  ["y"]=>
  int(3)
  ["m"]=>
  int(6)
  ["d"]=>
  int(4)
  ["h"]=>
  int(12)
  ["i"]=>
  int(30)
  ["s"]=>
  int(5)
  ["f"]=>
  float(0)
  ["invert"]=>
  int(0)
  ["days"]=>
  bool(false)
}
