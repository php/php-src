--TEST--
GH-24265 (exit() in a destroyed fiber is discarded while an exception is pending)
--FILE--
<?php

function test() {
    $fiber = new Fiber(function () {
        try {
            Fiber::suspend();
        } finally {
            exit(1);
        }
    });

    $fiber->start();
    echo "Before destroy", PHP_EOL;
    throw new Exception("pending");
}

try {
    test();
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "Not reached", PHP_EOL;

?>
--EXPECT--
Before destroy
