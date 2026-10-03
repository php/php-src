--TEST--
Bug GH-9916 009 (Entering shutdown sequence with a fiber suspended in a Generator emits an unavoidable fatal error or crashes) — under test_scheduler
--DESCRIPTION--
The expected output departs from the upstream
Zend/tests/fibers/gh9916-009.phpt. Under a scheduler the fiber still parked
when the script ends is ended by the scheduler with a graceful exit thrown at
its Fiber::suspend(), not destroyed with the objects. The generator is unwound
by that exit instead of being force-closed, so the yield from in its finally
block is allowed: it yields its first element back to $gen->current() in the
fiber's body, which goes on to print the line marked "Not executed". Upstream
throws "Cannot use yield from in a force-closed generator" there. The
scheduler RFC lists this behavior change as "A fiber parked in
Fiber::suspend() is ended by the scheduler, not collected".
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
