--TEST--
Pretty printing for arrow functions
--INI--
zend.assertions=1
--FILE--
<?php

try {
    assert((fn() => false)());
} catch (AssertionError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    assert((fn&(int... $args): ?bool => $args[0])(false));
} catch (AssertionError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
AssertionError: assert((fn() => false)())
AssertionError: assert((fn&(int ...$args): ?bool => $args[0])(false))
