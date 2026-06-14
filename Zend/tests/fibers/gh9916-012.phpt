--TEST--
Bug GH-9916 012 (Entering shutdown sequence with a fiber suspended in a Generator emits an unavoidable fatal error or crashes)
--FILE--
<?php

$gen = (function() {
    yield from (function() { yield; })();
})();
$fiber = new Fiber(function() use ($gen) {
    $gen->current();
});
$fiber->start();

?>
==DONE==
--EXPECT--
==DONE==
