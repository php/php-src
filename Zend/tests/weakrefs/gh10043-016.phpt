--TEST--
Self-referencing map entry GC - 016
--FILE--
<?php

class K1 { function __construct() {} }
class K2 {}

$map = new WeakMap();
$k1 = new K1();
$map[$k1] = [$k1, $map];

$k2 = new K2();
$map[$k2] = $k2;

gc_collect_cycles();

var_dump($map);
?>
--EXPECT--
object(WeakMap)#1 (2) {
  [0]=>
  array(2) {
    ["key"]=>
    object(K1)#2 (0) {
    }
    ["value"]=>
    array(2) {
      [0]=>
      object(K1)#2 (0) {
      }
      [1]=>
      *RECURSION*
    }
  }
  [1]=>
  array(2) {
    ["key"]=>
    object(K2)#3 (0) {
    }
    ["value"]=>
    object(K2)#3 (0) {
    }
  }
}
