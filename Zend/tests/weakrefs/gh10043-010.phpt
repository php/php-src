--TEST--
Self-referencing map entry GC - 010
--FILE--
<?php

class Value {
    function __construct() {
    }
}

function possibleRoot($value) {
}

$map = new WeakMap();

$obj = new stdClass();
possibleRoot($obj);

$obj2 = new Value();
$map[$obj2] = [$obj2, $map];

$obj = null;

$obj3 = new class {};
$map[$obj3] = $obj3;
unset($obj3);

gc_collect_cycles();
?>
==DONE==
--EXPECT--
==DONE==
