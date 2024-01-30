--TEST--
Bug #53437 (DateInterval basic serialization)
--FILE--
<?php
$di0 = new DateInterval('P2Y4DT6H8M');

$s = serialize($di0);

$di1 = unserialize($s);

var_dump($di0, $di1);

?>
--EXPECTF--
object(DateInterval)#1 (%d) {
  ["from_string"]=>
  bool(false)
  ["y"]=>
  int(2)
  ["m"]=>
  int(0)
  ["d"]=>
  int(4)
  ["h"]=>
  int(6)
  ["i"]=>
  int(8)
  ["s"]=>
  int(0)
  ["f"]=>
  float(0)
  ["invert"]=>
  int(0)
  ["days"]=>
  bool(false)
}
object(DateInterval)#2 (%d) {
  ["from_string"]=>
  bool(false)
  ["y"]=>
  int(2)
  ["m"]=>
  int(0)
  ["d"]=>
  int(4)
  ["h"]=>
  int(6)
  ["i"]=>
  int(8)
  ["s"]=>
  int(0)
  ["f"]=>
  float(0)
  ["invert"]=>
  int(0)
  ["days"]=>
  bool(false)
}
