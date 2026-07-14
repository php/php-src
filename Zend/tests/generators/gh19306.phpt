--TEST--
GH-19306: Generator suspended in yield from may be resumed
--FILE--
<?php

class It implements IteratorAggregate
{
    public function getIterator(): Generator
    {
        yield "";
        Fiber::suspend();
    }
}
function g()
{
    yield from new It();
}
$a = g();
$fiber = new Fiber(function () use ($a) {
    echo "Fiber start\n";
    $a->next();
    echo "Fiber return\n";
});
$fiber->start();
echo "Fiber suspended\n";
try {
    $a->next();
} catch (Throwable $t) {
    echo $t->getMessage(), "\n";
}
echo "Destroying fiber\n";
$fiber = null;
echo "Shutdown\n";
?>
--EXPECT--
Fiber start
Fiber suspended
Cannot resume an already running generator
Destroying fiber
Shutdown
