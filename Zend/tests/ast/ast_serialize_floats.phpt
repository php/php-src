--TEST--
Serialization of floats are correct
--INI--
zend.assertions=1
--FILE--
<?php
try {
    assert(!is_float(0.0));
} catch (AssertionError $e) {
    echo 'assert(): ', $e->getMessage(), ' failed', PHP_EOL;
}
try {
    assert(!is_float(1.1));
} catch (AssertionError $e) {
    echo 'assert(): ', $e->getMessage(), ' failed', PHP_EOL;
}
try {
    assert(!is_float(1234.5678));
} catch (AssertionError $e) {
    echo 'assert(): ', $e->getMessage(), ' failed', PHP_EOL;
}
?>
--EXPECT--
assert(): assert(!is_float(0.0)) failed
assert(): assert(!is_float(1.1)) failed
assert(): assert(!is_float(1234.5678)) failed
