--TEST--
Pretty printing for arrow functions
--INI--
zend.assertions=1
--FILE--
<?php

try {
    assert((fn() => false)());
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    assert((fn&(int... $args): ?bool => $args[0])(false));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
AssertionError: assert((fn() => false)())
AssertionError: assert((fn&(int ...$args): ?bool => $args[0])(false))
