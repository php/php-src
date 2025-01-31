--TEST--
list() with keys, evaluation order
--FILE--
<?php

require_once "list_keyed_evaluation_order.inc";

$a = new StringCapable("A");
$c = new StringCapable("C");

$e = new IndexableRetrievable("E", new Indexable(["A" => "value for offset A", "C" => "value for offset C"]));

$store = new Indexable([]);

// list($a => $b, $c => $d) = $e;
// Should be evaluated in the order:
// 1. Evaluate $e
// 2. Evaluate $a
// 3. Evaluate $e[$a]
// 4. Assign $b from $e[$a]
// 5. Evaluate $c
// 6. Evaluate $e[$c]
// 7. Assign $c from $e[$a]

list((string)$a => $store["B"], (string)$c => $store["D"]) = $e->getIndexable();

?>
--EXPECT--
Indexable E retrieved.
A evaluated.
Offset A retrieved.
Offset B set to value for offset A.
C evaluated.
Offset C retrieved.
Offset D set to value for offset C.
