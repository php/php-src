--TEST--
Bug GH-9916 007 (Entering shutdown sequence with a fiber suspended in a Generator emits an unavoidable fatal error or crashes)
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
        try {
            print "Before suspend\n";
            Fiber::suspend();
            print "Not executed\n";
        } finally {
            print "Finally (iterator)\n";
        }
    }

    public function valid(): bool
    {
        return true;
    }
};

$gen = (function() use ($it) {
    try {
        yield from $it;
        print "Not executed\n";
    } finally {
        print "Finally\n";
    }
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
Finally (iterator)
Finally
