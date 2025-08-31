--TEST--
WeakMap object reference offset
--FILE--
<?php

$map = new WeakMap;
$obj = new stdClass;
$obj2 = &$obj;

$map[$obj] = 1;
var_dump(count($map));
var_dump($map);
var_dump(isset($map[$obj]));
var_dump(!empty($map[$obj]));
var_dump($map[$obj]);

?>
--EXPECT--
int(1)
object(WeakMap)#1 (1) {
  [0]=>
  array(2) {
    ["key"]=>
    object(stdClass)#2 (0) {
    }
    ["value"]=>
    int(1)
  }
}
bool(true)
bool(true)
int(1)
