--TEST--
Bug GH-9916 003 (Entering shutdown sequence with a fiber suspended in a Generator emits an unavoidable fatal error or crashes)
--FILE--
<?php
$gen = (function() {
    $x = new stdClass;
    try {
        yield from (function () {
            $x = new stdClass;
            try {
                print "Before suspend\n";
                Fiber::suspend();
                print "Not executed\n";
                yield;
            } finally {
                print "Finally (inner)\n";
            }
        })();
        print "Not executed\n";
        yield;
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
Finally (inner)
Finally
