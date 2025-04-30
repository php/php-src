--TEST--
GH-15275 001: Crash during GC of suspended generator delegate
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
    var_dump(yield from new It());
}

$iterable = f();

$fiber = new Fiber(function () use ($iterable) {
    var_dump($iterable->current());
    $iterable->next();
    var_dump("not executed");
});

$ref = $fiber;

$fiber->start();

gc_collect_cycles();

?>
==DONE==
--EXPECT--
string(3) "foo"
==DONE==
