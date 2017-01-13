--TEST--
list() with keys, evaluation order: nested
--FILE--
<?php

require_once "list_keyed_evaluation_order.inc";

$a = new Stringable("A");
$c = new Stringable("C");
$f = new Stringable("F");
$g = new Stringable("G");
$i = new Stringable("I");

$k = new IndexableRetrievable("K", new Indexable([
    "A" => "offset value for A",
    "C" => new Indexable([
        0 => "offset value for 0",
        1 => "offset value for 1"
    ]),
    "F" => new Indexable([
        "G" => "offset value for G",
        "I" => "offset value for I"
    ])
]));

$store = new Indexable([]);

// list($a => $b, $c => list($d, $e), $f => list($g => $h, $i => $j)) = $k;
// Should be evaluated in the order:
// 1. Evaluate $k
// 2. Evaluate $a
// 3. Evaluate $k[$a]
// 4. Assign $b from $k[$a]
// 5. Evaluate $c
// 6. Evaluate $k[$c]
// 7. Evaluate $k[$c][0]
// 8. Assign $d from $k[$c][0]
// 9. Evaluate $k[$c][1]
// 10. Assign $e from $k[$c][1]
// 11. Evaluate $f
// 12. Evaluate $k[$f]
// 13. Evaluate $g
// 14. Evaluate $k[$f][$g]
// 15. Assign $h from $k[$f][$g]
// 16. Evaluate $i
// 17. Evaluate $k[$f][$i]
// 18. Assign $j from $k[$f][$i]

list(
    (string)$a => $store["B"],
    (string)$c => list($store["D"], $store["E"]),
    (string)$f => list(
        (string)$g => $store["H"],
        (string)$i => $store["J"]
    )
) = $k->getIndexable();

?>
--EXPECT--
Indexable K retrieved.
A evaluated.
Offset A retrieved.
Offset B set to offset value for A.
C evaluated.
Offset C retrieved.
Offset 0 retrieved.
Offset D set to offset value for 0.
Offset 1 retrieved.
Offset E set to offset value for 1.
F evaluated.
Offset F retrieved.
G evaluated.
Offset G retrieved.
Offset H set to offset value for G.
I evaluated.
Offset I retrieved.
Offset J set to offset value for I.
