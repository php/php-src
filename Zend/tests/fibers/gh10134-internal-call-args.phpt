--TEST--
GH-10134 (GC sees the arguments of an internal call a fiber is suspended inside)
--FILE--
<?php

// No generator involved: the fiber is only kept alive by the array argument of
// the internal array_map() frame it is suspended inside.
class Canary {
    public function __destruct() {
        var_dump('Canary dtor');
    }
}

$canary = new Canary();
$fiber = new Fiber(function() use (&$fiber, $canary) {
    array_map(function($x) { Fiber::suspend(); return $x; }, [$fiber]);
});
$fiber->start();

$fiber = null;
$canary = null;
var_dump(gc_collect_cycles() > 0);

var_dump('Shutdown');

?>
--EXPECT--
string(11) "Canary dtor"
bool(true)
string(8) "Shutdown"
