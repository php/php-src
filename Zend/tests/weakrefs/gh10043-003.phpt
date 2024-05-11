--TEST--
Self-referencing map entry GC - 003
--FILE--
<?php

class Value {
    public function __construct(public readonly string $value) {
    }
}

$map = new WeakMap();
$obj = new Value('a');
$map[$obj] = [$obj, $map];
$ref = WeakReference::create($map);

gc_collect_cycles();

var_dump($ref->get());

gc_collect_cycles();

// $obj is first in the root buffer
$obj = null;
$map = null;
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
      object(Value)#%d (1) {
        ["value"]=>
        string(1) "a"
      }
      [1]=>
      *RECURSION*
    }
  }
}
NULL
