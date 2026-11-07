--TEST--
Cycles between tasks and futures are collected
--FILE--
<?php
class Holder { public $future; }

for ($i = 0; $i < 3; $i++) {
    $h = new Holder;
    $h->future = Async\async(function () use ($h) {
        await Async\delay(0);
        return $h;
    });
    await $h->future;
    unset($h);
}
var_dump(gc_collect_cycles() > 0);
?>
--EXPECT--
bool(true)
