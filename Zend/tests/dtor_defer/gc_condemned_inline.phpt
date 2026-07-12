--TEST--
Deferred destructor: a drop inside a GC destructor of a condemned sibling runs inline
--FILE--
<?php
$log = 0;
class Node {
    public $next;
    public $peer;
    function __destruct() {
        global $log;
        $log++;
        $this->peer = null;
    }
}

for ($r = 0; $r < 2000; $r++) {
    $a = new Node();
    $b = new Node();
    $c = new Node();
    $a->next = $b;
    $b->next = $c;
    $c->next = $a;
    $a->peer = $c;
    $b->peer = $a;
    $c->peer = $b;
    unset($a, $b, $c);
    if ($r % 500 === 0) {
        gc_collect_cycles();
    }
}
gc_collect_cycles();
echo "log=$log\n";
?>
--EXPECT--
log=6000
