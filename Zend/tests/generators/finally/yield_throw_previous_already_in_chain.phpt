--TEST--
Exception thrown in finally of a generator destroyed during unwinding that already has the pending exception in its previous chain
--FILE--
<?php
function gen(Throwable $pending) {
    try {
        yield;
    } finally {
        throw new LogicException("thrown", 0, $pending);
    }
}

function f() {
    $pending = new RuntimeException("pending");
    $generator = gen($pending);
    $generator->current();
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
