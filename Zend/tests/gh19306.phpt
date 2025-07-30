--TEST--
GH-19306: Fix dangling stack frames with fibers and nested generators
--FILE--
<?php

class It implements IteratorAggregate {
    public function getIterator(): Generator {
        yield "";
        Fiber::suspend();
    }
}

function g() {
    yield from new It();
}

$b = g();
$b->rewind();

$fiber = new Fiber(function () use ($b) {
    $b->next();
});

$fiber->start();

try {
    $b->next();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
