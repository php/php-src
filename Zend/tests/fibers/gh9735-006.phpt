--TEST--
Bug GH-9735 006 (Fiber stack variables do not participate in cycle collector)
--FILE--
<?php

class C {
    public function __destruct() {
        echo __METHOD__, "\n";
    }
}

function f() {
    // Force symbol table
    get_defined_vars();

    Fiber::suspend();
}

$fiber = new Fiber(function () {
    $c = new C();

    $fiber = Fiber::getCurrent();

    // Force symbol table
    get_defined_vars();

    f();
});

print "1\n";

$fiber->start();
gc_collect_cycles();

print "2\n";

$fiber = null;
gc_collect_cycles();

print "3\n";

?>
--EXPECTF--
1
2
C::__destruct
3
