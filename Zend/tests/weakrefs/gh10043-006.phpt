--TEST--
Self-referencing map entry GC - 006
--FILE--
<?php

class Value {
    public function __construct(public readonly string $value) {
    }
}

$map = new WeakMap();
$obj = new Value('a');
$map[$obj] = $obj;

gc_collect_cycles();

$obj2 = $obj;
$obj = null;
$map2 = $map;
$map = null;

gc_collect_cycles();

var_dump($map2);

?>
--EXPECT--
object(WeakMap)#1 (1) {
  [0]=>
  array(2) {
    ["key"]=>
    object(Value)#2 (1) {
      ["value"]=>
      string(1) "a"
    }
    ["value"]=>
    object(Value)#2 (1) {
      ["value"]=>
      string(1) "a"
    }
  }
}
