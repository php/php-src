--TEST--
GH-14983: Guards from fibers are separated from guards from main context
--FILE--
<?php

class Foo {
    public function __get($prop) {
        echo "__set($prop)\n";
        if (Fiber::getCurrent()) {
            Fiber::suspend();
            echo "Resuming\n";
        }
    }
}

$foo = new Foo();

$fiber = new Fiber(function () use ($foo) {
    $foo->bar;
});

$value = $fiber->start();
echo "Suspended\n";
$foo->bar;
$fiber->resume('test');

?>
--EXPECT--
__set(bar)
Suspended
__set(bar)
Resuming
