--TEST--
Closure::fromCallable() and GC
--FILE--
<?php

class Test {
    public function method() {}

    public function method2($y) {
        static $x;
        $x = $y;
    }
}

$fn = Closure::fromCallable([new Test, 'method2']);
$fn($fn);
unset($fn); // Still referenced from static var.
gc_collect_cycles();

$fn = Closure::fromCallable([new Test, 'method']);
$fn2 = $fn; unset($fn2); // Add to root buffer.
gc_collect_cycles();

?>
===DONE===
--EXPECT--
===DONE===
