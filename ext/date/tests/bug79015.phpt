--TEST--
Bug #79015 (undefined-behavior in php_date.c)
--FILE--
<?php
$payload = 'O:12:"DateInterval":9:{s:1:"y";i:1;s:1:"m";i:0;s:1:"d";i:4;s:1:"h";i:0;s:1:"i";i:0;s:1:"s";i:0;s:1:"f";i:9999999999990;s:6:"invert";i:0;s:4:"days";b:0;}';
var_dump(unserialize($payload));
?>
--EXPECTF--
object(DateInterval)#%d (%d) {
  ["y"]=>
  int(1)
  ["m"]=>
  int(0)
  ["d"]=>
  int(4)
  ["h"]=>
  int(0)
  ["i"]=>
  int(0)
  ["s"]=>
  int(0)
  ["f"]=>
  float(%f)
  ["invert"]=>
  int(0)
  ["days"]=>
  bool(false)
  ["from_string"]=>
  bool(false)
  ["date_string"]=>
  NULL
}
