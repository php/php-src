--TEST--
Pretty printing for arrow functions
--INI--
zend.assertions=1
--FILE--
<?php

try {
    assert((fn() => false)());
} catch (AssertionError $e) {
    echo 'assert(): ', $e->getMessage(), ' failed', PHP_EOL;
}

try {
    assert((fn&(int... $args): ?int => $args[0])(0));
} catch (AssertionError $e) {
    echo 'assert(): ', $e->getMessage(), ' failed', PHP_EOL;
}

?>
--EXPECT--
assert(): assert((fn() => false)()) failed
assert(): assert((fn&(int ...$args): ?int => $args[0])(0)) failed
