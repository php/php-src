--TEST--
Bug GH-9916 009 (Entering shutdown sequence with a fiber suspended in a Generator emits an unavoidable fatal error or crashes)
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
--EXPECTF--
Before suspend
==DONE==
Finally

Fatal error: Uncaught Error: Cannot use "yield from" in a force-closed generator in %s:%d
Stack trace:
#0 [internal function]: {closure}()
#1 %s(%d): Generator->current()
#2 [internal function]: {closure}()
#3 {main}
  thrown in %s on line %d
