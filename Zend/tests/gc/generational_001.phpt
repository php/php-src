--TEST--
Generational GC: node promoted to old gen while being rooted must not be added to the young buffer
--ENV--
FULL_GC_FREQ=10
--FILE--
<?php

class Node {
    public $owner;
}

$c = new stdClass;
$c->nodes = [];

// Re-assigning $n makes the previous Node a possible root. This eventually
// triggers a GC run from gc_possible_root_when_full(). The node is promoted to
// the old gen.
while (gc_status()['threshold'] - gc_status()['roots'] > 1) {
    $n = new Node;
    $n->owner = $c;
    $c->nodes[] = $n;
}
unset($n);

var_dump(gc_status()['runs']);

// Young GC: must not collect anything, as everything is reachable from $c.
var_dump(gc_collect_cycles(false));

?>
--EXPECT--
int(1)
int(0)
