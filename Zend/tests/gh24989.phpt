--TEST--
GH-24989 (Use-after-free in GC when an exception is thrown)
--CREDITS--
vi3tL0u1s
--FILE--
<?php
class D { function __destruct() { gc_collect_cycles(); } }

function loop_var() {
    foreach (range(1, 2) as $v) {
        $x = [new D, throw new Exception('loop')];
    }
}

function tmp_var() {
    $x = [range(1, 2), [new D, throw new Exception('tmp')]];
}

foreach (['loop_var', 'tmp_var'] as $f) {
    try {
        $f();
    } catch (Exception $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}
?>
--EXPECT--
Exception: loop
Exception: tmp
