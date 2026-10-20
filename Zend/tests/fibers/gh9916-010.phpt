--TEST--
Bug GH-9916 010 (Entering shutdown sequence with a fiber suspended in a Generator emits an unavoidable fatal error or crashes)
--FILE--
<?php
$gen = (function() {
    $x = new stdClass;
    print "Before yield\n";
    yield from (function () {
        $x = new stdClass;
        print "Before yield 2\n";
        yield;
        print "Before suspend\n";
        Fiber::suspend();
    })();
})();

$fiber = new Fiber(function () use ($gen, &$fiber) {
    print "Before current\n";
    $gen->current();
    print "Before next\n";
    $gen->next();
    print "Not executed\n";
});

$fiber->start();
?>
==DONE==
--EXPECT--
Before current
Before yield
Before yield 2
Before next
Before suspend
==DONE==
