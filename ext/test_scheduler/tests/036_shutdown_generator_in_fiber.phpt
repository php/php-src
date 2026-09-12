--TEST--
Bug GH-9916 009 (Entering shutdown sequence with a fiber suspended in a Generator emits an unavoidable fatal error or crashes) — under test_scheduler
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
$gen = (function() {
    $x = new stdClass;
    try {
        print "Before suspend\n";
        Fiber::suspend();
        print "Not executed\n";
    } finally {
        print "Finally\n";
        yield from ['foo' => new stdClass];
        print "Not executed\n";
    }
})();
$fiber = new Fiber(function() use ($gen, &$fiber) {
    $gen->current();
    print "Not executed\n";
});
$fiber->start();
?>
==DONE==
--EXPECT--
Before suspend
==DONE==
Finally
Not executed
