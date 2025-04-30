--TEST--
GH-15108 002: Segfault with delegated generator in suspended fiber
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

function g() {
    yield from new It();
}

function f() {
    yield from g();
}

$iterable = f();

$fiber = new Fiber(function () use ($iterable) {
    var_dump($iterable->current());
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
