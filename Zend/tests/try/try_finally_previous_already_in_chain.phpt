--TEST--
Exception thrown in finally that already has the pending exception in its previous chain
--FILE--
<?php
function direct() {
    $e = new RuntimeException("pending");
    try {
        throw $e;
    } finally {
        throw new LogicException("thrown", 0, $e);
    }
}

function nested() {
    $e = new RuntimeException("pending");
    try {
        throw $e;
    } finally {
        throw new LogicException("thrown", 0, new Exception("middle", 0, $e));
    }
}

foreach (['direct', 'nested'] as $function) {
    try {
        $function();
    } catch (Throwable $t) {
        for (; $t !== null; $t = $t->getPrevious()) {
            echo $t->getMessage(), "\n";
        }
    }
}
?>
--EXPECT--
thrown
pending
thrown
middle
pending
