--TEST--
GH-15108 004: Segfault with delegated generator in suspended fiber
--FILE--
<?php

function gen1() {
    yield 'foo';
    Fiber::suspend();
    var_dump("not executed");
};

function gen2($gen) {
    yield from $gen;
    var_dump("not executed");
}

$a = gen1();
/* Both $b and $c have a root marked with IN_FIBER, but only $b is actually
 * running in a fiber (at shutdown) */
$b = gen2($a);
$c = gen2($a);

$fiber = new Fiber(function () use ($a, $b, $c) {
    var_dump($b->current());
    var_dump($c->current());
    $b->next();
    var_dump("not executed");
});

$ref = $fiber;

$fiber->start();

?>
==DONE==
--EXPECT--
string(3) "foo"
string(3) "foo"
==DONE==
