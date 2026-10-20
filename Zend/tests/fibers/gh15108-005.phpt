--TEST--
GH-15108 005: Segfault with delegated generator in suspended fiber
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

function h() {
    /* g() is an intermediate node and will not be marked with IN_FIBER */
    yield from g();
}

$iterable = h();
var_dump($iterable->current());

$fiber = new Fiber(function () use ($iterable) {
    $iterable->next();
    var_dump("not executed");
});

$ref = $fiber;

$fiber->start();

?>
==DONE==
--EXPECT--
string(3) "foo"
==DONE==
