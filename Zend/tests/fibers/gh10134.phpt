--TEST--
GH-10134 (Non-suspended generators in suspended Fiber do not participate in GC)
--FILE--
<?php

class Canary {
    public function __construct(
        public readonly string $x,
    ) {
    }

    public function __destruct() {
        var_dump($this->x);
    }
}

$gen = (function() {
    $canary = new Canary('Generator dtor');
    $fiber = yield;
    Fiber::suspend();
})();

$fiber = new Fiber(function() use ($gen, &$fiber) {
    $canary = new Canary('Fiber dtor');
    $gen->send($fiber);
});
$fiber->start();

$gen = null;
$fiber = null;
gc_collect_cycles();

var_dump('Shutdown');

?>
--EXPECT--
string(14) "Generator dtor"
string(10) "Fiber dtor"
string(8) "Shutdown"
