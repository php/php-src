--TEST--
Bug GH-9916 002 (Entering shutdown sequence with a fiber suspended in a Generator emits an unavoidable fatal error or crashes)
--FILE--
<?php
$gen = (function() {
    $x = new stdClass;
    print "Before suspend\n";
    Fiber::suspend();
    print "Not executed\n";
    yield;
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
