--TEST--
Bug GH-9916 005 (Entering shutdown sequence with a fiber suspended in a Generator emits an unavoidable fatal error or crashes)
--FILE--
<?php
$gen = (function() {
    $x = new stdClass;
    $fiber = yield;
    print "Before suspend\n";
    Fiber::suspend();
    yield;
})();
$fiber = new Fiber(function() use ($gen, &$fiber) {
    $gen->send($fiber);
    $gen->current();
});
$fiber->start();

$gen = null;
$fiber = null;
gc_collect_cycles();
?>
==DONE==
--EXPECT--
Before suspend
==DONE==
