--TEST--
GH-15108 006: Segfault with delegated generator in suspended fiber
--FILE--
<?php

class It implements \IteratorAggregate
{
    public function getIterator(): \Generator
    {
        yield 'foo';
        Fiber::suspend();
        var_dump("not executed");
    }
}

function f() {
    yield from new It();
}

function g() {
    yield from f();
}

function gen($gen) {
    /* $gen is an intermediate node and will not be marked with IN_FIBER */
    yield from $gen;
}

$g = g();
$a = gen($g);
$b = gen($g);
var_dump($a->current());
var_dump($b->current());

$fiber = new Fiber(function () use ($a, $b, $g) {
    $a->next();
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
