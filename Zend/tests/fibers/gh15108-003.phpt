--TEST--
GH-15108 003: Segfault with delegated generator in suspended fiber
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

function f($gen) {
    yield from $gen;
}

$a = new It();
$b = f($a);
$c = f($a);

$fiber = new Fiber(function () use ($a, $b, $c) {
    var_dump($b->current());
    $b->next();
    var_dump("not executed");
});

$ref = $fiber;

$fiber->start();

?>
==DONE==
--EXPECT--
string(3) "foo"
==DONE==
