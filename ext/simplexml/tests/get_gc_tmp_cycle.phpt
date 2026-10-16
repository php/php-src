--TEST--
SimpleXMLElement: a cycle through the by-reference property of a missing element is collected
--EXTENSIONS--
simplexml
--FILE--
<?php
$x = simplexml_load_string('<r/>');
$w = WeakReference::create($x);
$r = &$x->n;
$r = $x;
unset($r, $x);
gc_collect_cycles();
var_dump($w->get() === null);

function g(&$o, $v) { $o = $v; }
$x = simplexml_load_string('<r/>');
$w = WeakReference::create($x);
g($x->n, $x);
unset($x);
gc_collect_cycles();
var_dump($w->get() === null);
?>
--EXPECT--
bool(true)
bool(true)
