--TEST--
Exception thrown in a destructor during unwinding that already has the pending exception in its previous chain
--FILE--
<?php
class ThrowsInDestructor {
    public function __construct(private Throwable $pending) {}

    public function __destruct() {
        throw new LogicException("thrown", 0, $this->pending);
    }
}

function f() {
    $pending = new RuntimeException("pending");
    $object = new ThrowsInDestructor($pending);
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
