--TEST--
Self-referencing map entry GC - 002
--FILE--
<?php

class Value {
    public function __construct(public readonly string $value) {
    }
}

$map = new WeakMap();
$obj = new Value('a');
$map[$obj] = [$obj];

gc_collect_cycles();

var_dump($map);

$obj = null;
gc_collect_cycles();

var_dump($map);

--EXPECTF--
object(WeakMap)#%d (1) {
  [0]=>
  array(2) {
    ["key"]=>
    object(Value)#%d (1) {
      ["value"]=>
      string(1) "a"
    }
    ["value"]=>
    array(1) {
      [0]=>
      object(Value)#%d (1) {
        ["value"]=>
        string(1) "a"
      }
    }
  }
}
object(WeakMap)#1 (0) {
}
