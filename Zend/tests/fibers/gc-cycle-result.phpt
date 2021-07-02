--TEST--
GC can cleanup cycle when fiber result references fiber
--FILE--
<?php

$fiber = null;
$fiber = new Fiber(function () use (&$fiber) {
    return new class($fiber) {
        private $fiber;
        
        public function __construct($fiber) {
            $this->fiber = $fiber;
        }
        
        public function __destruct() {
            var_dump('DTOR');
        }
    };
});

$fiber->start();

var_dump('COLLECT CYCLES');
gc_collect_cycles();
var_dump('DONE');

var_dump($fiber->isTerminated());

unset($fiber);

var_dump('COLLECT CYCLES');
gc_collect_cycles();
var_dump('DONE');

?>
--EXPECT--
string(14) "COLLECT CYCLES"
string(4) "DONE"
bool(true)
string(14) "COLLECT CYCLES"
string(4) "DTOR"
string(4) "DONE"
