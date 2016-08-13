--TEST--
SPL: Bug #70155 Use After Free Vulnerability in unserialize() with SPLArrayObject
--FILE--
<?php
$inner = 'x:i:0;O:12:"DateInterval":1:{s:1:"y";i:3;};m:a:1:{i:0;R:2;}';
$exploit = 'C:11:"ArrayObject":'.strlen($inner).':{'.$inner.'}';
$data = unserialize($exploit);

var_dump($data);
?>
===DONE===
--EXPECTF--
object(ArrayObject)#1 (2) {
  [0]=>
  int(0)
  ["storage":"ArrayObject":private]=>
  object(DateInterval)#2 (15) {
    ["y"]=>
    int(3)
    ["m"]=>
    int(-1)
    ["d"]=>
    int(-1)
    ["h"]=>
    int(-1)
    ["i"]=>
    int(-1)
    ["s"]=>
    int(-1)
    ["weekday"]=>
    int(-1)
    ["weekday_behavior"]=>
    int(-1)
    ["first_last_day_of"]=>
    int(-1)
    ["invert"]=>
    int(0)
    ["days"]=>
    int(-1)
    ["special_type"]=>
    int(0)
    ["special_amount"]=>
    int(-1)
    ["have_weekday_relative"]=>
    int(0)
    ["have_special_relative"]=>
    int(0)
  }
}
===DONE===
