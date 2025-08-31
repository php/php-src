--TEST--
Self-referencing map entry GC - 009
--FILE--
<?php

class Value {
    public function __construct(public readonly string $value) {
    }
}

function possibleRoot($var) {
}

$map = new WeakMap();
$obj = new stdClass();
$map[$obj] = new Value('a');
$map[$map] = $map;
$ref = WeakReference::create($map);

possibleRoot($obj);
$map = null;

gc_collect_cycles();

var_dump($ref->get());
?>
--EXPECT--
NULL
