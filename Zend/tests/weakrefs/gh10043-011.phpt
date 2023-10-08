--TEST--
Self-referencing map entry GC - 011
--FILE--
<?php

class K {}
class V { public $k; }

$m = new WeakMap();
$k = new K;
$v = new V;
$v->k = $k;
$m[$k] = $v;

$m2 = $m;
unset($m2, $k, $v);

gc_collect_cycles();

var_dump($m);

--EXPECT--
object(WeakMap)#1 (0) {
}
