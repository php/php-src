--TEST--
GH-11028 (Heap Buffer Overflow in zval_undefined_cv with generators) - throw in finally variant
--FILE--
<?php
function generator() {
    try {
        yield null => 0;
    } finally {
        throw new Exception("exception");
        return [];
    }
}

try {
    var_dump([...generator()]);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
Deprecated: Returning from a finally block is deprecated in %s on line %d
exception