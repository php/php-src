--TEST--
GC can cleanup cycle when callback references fiber
--FILE--
<?php

$ref = new class () {
    public $fiber;
    
    public function __destruct() {
        var_dump('DTOR');
    }
};

$fiber = new Fiber(function () use ($ref) {
    die('UNREACHABLE');
});

$ref->fiber = $fiber;

$fiber = null;
$ref = null;

var_dump('COLLECT CYCLES');
gc_collect_cycles();
var_dump('DONE');

?>
--EXPECT--
string(14) "COLLECT CYCLES"
string(4) "DTOR"
string(4) "DONE"
