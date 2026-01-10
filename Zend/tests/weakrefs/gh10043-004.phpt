--TEST--
Self-referencing map entry GC - 004
--FILE--
<?php

class Value {
    public function __construct(public readonly string $value) {
    }
}

$map = new WeakMap();
$obj = new Value('a');
$map[$obj] = [$map, $obj];
$ref = WeakReference::create($map);

gc_collect_cycles();

var_dump($ref->get());

gc_collect_cycles();

// $map is first in the root buffer
$map = null;
$obj = null;
gc_collect_cycles();

var_dump($ref->get());

?>
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
    array(2) {
      [0]=>
      *RECURSION*
      [1]=>
      object(Value)#%d (1) {
        ["value"]=>
        string(1) "a"
      }
    }
  }
}
NULL
