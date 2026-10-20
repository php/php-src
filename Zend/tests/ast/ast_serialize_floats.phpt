--TEST--
Serialization of floats are correct
--INI--
zend.assertions=1
--FILE--
<?php
try {
    assert(!is_float(0.0));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    assert(!is_float(1.1));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    assert(!is_float(1234.5678));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
AssertionError: assert(!is_float(0.0))
AssertionError: assert(!is_float(1.1))
AssertionError: assert(!is_float(1234.5678))
