--TEST--
GH-18304 (Changing the properties of a DateInterval through dynamic properties triggers a SegFault)
--CREDITS--
orose-assetgo
--FILE--
<?php
$di = new \DateInterval('P0Y');
$field = 'd';
$i = 1;
$di->$field += $i;
var_dump($di);
?>
--EXPECT--
object(DateInterval)#1 (10) {
  ["y"]=>
  int(0)
  ["m"]=>
  int(0)
  ["d"]=>
  int(1)
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
  bool(false)
  ["from_string"]=>
  bool(false)
}
