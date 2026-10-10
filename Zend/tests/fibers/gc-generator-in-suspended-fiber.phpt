--TEST--
GC collects a cycle through a generator parked in Fiber::suspend() on a fiber stack
--FILE--
<?php

class C {
    public function __destruct() {
        echo __METHOD__, "\n";
    }
}

function gen() {
    $c = new C();
    $self = Fiber::getCurrent(); // cycle: fiber -> frames -> $self -> fiber
    yield 1;
    Fiber::suspend("from gen");
    yield 2;
}

$fiber = new Fiber(function () {
    $g = gen();
    $g->current();
    $g->next(); // parks in Fiber::suspend() inside the generator
    echo "unreachable\n";
});

$fiber->start();
print "1\n";

// Still referenced: nothing to collect.
gc_collect_cycles();
print "2\n";

$fiber = null;
gc_collect_cycles();
print "3\n";
?>
--EXPECT--
1
2
C::__destruct
3
