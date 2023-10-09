--TEST--
Bug GH-9916 006 (Entering shutdown sequence with a fiber suspended in a Generator emits an unavoidable fatal error or crashes)
--FILE--
<?php
$gen = (function() {
    $x = new stdClass;
    yield from (function () {
        $x = new stdClass;
        print "Before suspend\n";
        Fiber::suspend();
        print "After suspend\n";
        yield;
    })();
    yield from (function () {
        $x = new stdClass;
        print "Before exit\n";
        exit;
        print "Not executed\n";
        yield;
    })();
    yield;
})();
$fiber = new Fiber(function() use ($gen, &$fiber) {
    $gen->current();
    print "Fiber return\n";
});
$fiber->start();
$fiber->resume();
$gen->next();
$gen->current();
?>
==DONE==
--EXPECT--
Before suspend
After suspend
Fiber return
Before exit
