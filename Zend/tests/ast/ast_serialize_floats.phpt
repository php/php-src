--TEST--
Serialization of floats are correct
--INI--
zend.assertions=1
--FILE--
<?php
try {
    assert(!is_float(0.0));
} catch (AssertionError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    assert(!is_float(1.1));
} catch (AssertionError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    assert(!is_float(1234.5678));
} catch (AssertionError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
AssertionError: assert(!is_float(0.0))
AssertionError: assert(!is_float(1.1))
AssertionError: assert(!is_float(1234.5678))
