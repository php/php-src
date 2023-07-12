--TEST--
Pretty printing for arrow functions
--INI--
zend.assertions=1
--FILE--
<?php

// TODO We're missing parentheses for the direct call

try {
    assert((fn() => false)());
} catch (AssertionError $e) {
    echo 'assert(): ', $e->getMessage(), ' failed', PHP_EOL;
}

try {
    assert((fn&(int... $args): ?bool => $args[0])(false));
} catch (AssertionError $e) {
    echo 'assert(): ', $e->getMessage(), ' failed', PHP_EOL;
}

?>
--EXPECT--
assert(): assert(fn() => false()) failed
assert(): assert(fn&(int ...$args): ?bool => $args[0](false)) failed
