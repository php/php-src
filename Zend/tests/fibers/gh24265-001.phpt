--TEST--
GH-24265 (exit() in a finally block of a destroyed fiber is discarded)
--CREDITS--
iluuu1994
--FILE--
<?php

$fiber = new Fiber(function () {
    try {
        Fiber::suspend();
    } finally {
        exit(1);
    }
});

$fiber->start();
echo "Before destroy", PHP_EOL;
unset($fiber);

echo "Not reached", PHP_EOL;

?>
--EXPECT--
Before destroy
