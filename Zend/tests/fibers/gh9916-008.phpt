--TEST--
Bug GH-9916 008 (Entering shutdown sequence with a fiber suspended in a Generator emits an unavoidable fatal error or crashes)
--FILE--
<?php
$it = new class implements Iterator
{
    public function current(): mixed
    {
        return null;
    }

    public function key(): mixed
    {
        return 0;
    }

    public function next(): void
    {
    }

    public function rewind(): void
    {
        $x = new stdClass;
        print "Before suspend\n";
        Fiber::suspend();
    }

    public function valid(): bool
    {
        return true;
    }
};

$gen = (function() use ($it) {
    $x = new stdClass;
    yield from $it;
})();
$fiber = new Fiber(function() use ($gen, &$fiber) {
    $gen->current();
    print "Not executed";
});
$fiber->start();
?>
==DONE==
--EXPECT--
Before suspend
==DONE==
