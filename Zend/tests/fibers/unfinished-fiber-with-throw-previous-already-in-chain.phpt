--TEST--
Exception thrown in finally of a fiber destroyed during unwinding that already has the pending exception in its previous chain
--FILE--
<?php
function f() {
    $pending = new RuntimeException("pending");
    $fiber = new Fiber(function () use ($pending) {
        try {
            Fiber::suspend();
        } finally {
            throw new LogicException("thrown", 0, $pending);
        }
    });
    $fiber->start();
    throw $pending;
}

try {
    f();
} catch (Throwable $t) {
    for (; $t !== null; $t = $t->getPrevious()) {
        echo $t->getMessage(), "\n";
    }
}
?>
--EXPECT--
thrown
pending
